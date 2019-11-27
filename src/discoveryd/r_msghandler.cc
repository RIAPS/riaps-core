#include <discoveryd/r_msghandler.h>
#include <framework/rfw_configuration.h>
#include <framework/rfw_network_interfaces.h>
#include <framework/rfw_security.h>
#include <discoveryd/r_msghandler.h>
#include <discoveryd/r_dhttracker.h>
#include <utils/r_lmdb.h>

using namespace std;

#define DBGLN logger_->debug("{}::{}", __FUNCTION__, __LINE__);

namespace riaps{
    namespace discovery {
        DiscoveryMessageHandler::DiscoveryMessageHandler(dht::DhtRunner &dht_node,
                                                         zsock_t **pipe,
                                                         shared_ptr<spdlog::logger> logger)
                : dht_node_(dht_node),
                  pipe_(*pipe),
                  service_renewal_period_((uint16_t) 20000), // 20 sec in in msec.
                  zombie_renewal_period_((uint16_t) 600000), // 10 min in msec
                  mac_address_(riaps::framework::Network::GetMacAddressStripped()),
                  host_address_(riaps::framework::Network::GetIPAddress()),
                  zombieglobalkey_("/zombie/globals"),
                  zombielocalkey_(fmt::format("/zombie/locals/{}", mac_address_)),
                  terminated_(false),
                  logger_(logger),
                  rep_identity_(nullptr) {
            has_security_ = riaps::framework::Security::HasSecurity();
        }

        bool DiscoveryMessageHandler::Init() {
            if (has_security_)
                private_key_ = framework::Security::private_key();

            dht_update_socket_ = zsock_new_pull(DHT_RESULT_CHANNEL);
            zsock_set_rcvtimeo(dht_update_socket_, 0);

            riaps_socket_ = zsock_new(ZMQ_ROUTER);

            zsock_set_linger(riaps_socket_, 0);
            zsock_set_sndtimeo(riaps_socket_, 0);
            zsock_set_rcvtimeo(riaps_socket_, 0);

            dht_tracker_ = zactor_new(dht_tracker, &dht_node_);

            zsock_bind(riaps_socket_, "%s", riaps::framework::Configuration::GetDiscoveryEndpoint().c_str());

            poller_ = zpoller_new(pipe_, dht_update_socket_, riaps_socket_, nullptr);
            zpoller_set_nonstop(poller_, true);

            last_service_renewal_ = last_zombie_renewal_ = zclock_mono();

            // Get current global zombies
            dht_node_.get<DhtData>(dht::InfoHash::get(zombieglobalkey_), [this](vector<DhtData> &&values) {
                async(launch::async, &DiscoveryMessageHandler::HandleZombieUpdate, this, values);
                return true;
            });

            // Get local zombies
            dht_node_.get<DhtData>(dht::InfoHash::get(zombielocalkey_), [this](vector<DhtData> &&values) {
                async(launch::async, &DiscoveryMessageHandler::HandleZombieUpdate, this, values);
                return true;
            });

            // Subscribe for further zombies
            dht_node_.listen<DhtData>(dht::InfoHash::get(zombieglobalkey_), [this](vector<DhtData> &&values) {
                async(launch::async, &DiscoveryMessageHandler::HandleZombieUpdate, this, values);
                return true;
            });

            dht_node_.listen<DhtData>(dht::InfoHash::get(zombielocalkey_), [this](vector<DhtData> &&values) {
                async(launch::async, &DiscoveryMessageHandler::HandleZombieUpdate, this, values);
                return true;
            });

            // The discovery service may be restarted, previously registered response ports are put under the zombie key
            try {
                auto db = Lmdb::db();
                auto services = db->GetAll();

                for (auto &service : *services) {
                    auto key = get<0>(service);
                    auto address = get<1>(service);
                    vector<uint8_t> opendht_data(address.begin(), address.end());
                    logger_->info("Mark service {}:{} as zombie", key, address);
                    if (address.find("127.0.0.1") != string::npos) {
                        DhtPut(zombielocalkey_, opendht_data);
                    } else {
                        DhtPut(zombieglobalkey_, opendht_data);
                    }
                    db->Del(key);
                }
            } catch (lmdb_error &e) {
                logger_->error("{}", e.what());
            }
        }

        future<bool> DiscoveryMessageHandler::WaitForDht() {
            auto &logger = logger_;
            return async(launch::async, [logger]() -> bool {
                zsock_t *query = zsock_new(ZMQ_DEALER);
                zuuid_t *socket_id = zuuid_new();
                zsock_set_identity(query, zuuid_str(socket_id));
                zsock_connect(query, CHAN_IN_DHTTRACKER);

                // Retry
                auto retry = 10;
                uint8_t result = 0;
                while (retry-- > 0 && !result) {
                    zsock_send(query, "ss", CMD_QUERY_STABLE, "void");
                    zsock_recv(query, "1", &result);

                    if (!result) {
                        logger->info("DHT is not ready.");
                        zclock_sleep(500);
                    }
                }
                zuuid_destroy(&socket_id);
                zsock_destroy(&query);
                return result == 1;
            });
        }

        void DiscoveryMessageHandler::Run() {
            while (!terminated_) {
                void *which = zpoller_wait(poller_, REGULAR_MAINTAIN_PERIOD);

                // Check services whether they are still alive.
                // Reregister the too old services (OpenDHT ValueType settings, it is 10 minutes by default)
                int64_t loop_start_time = zclock_mono();
                if ((loop_start_time - last_service_renewal_) > service_renewal_period_) {
                    RenewServices();
                }

                // Check outdated zombies
                if ((loop_start_time - last_zombie_renewal_) > zombie_renewal_period_) {
                    MaintainZombieList();
                }

                // Handling messages from the caller (e.g.: $TERM$)
                if (which == pipe_) {
                    HandlePipeMessage();
                } else if (which == dht_update_socket_) {
                    // Process the updated nodes
                    zmsg_t *msg_response = zmsg_recv(dht_update_socket_);

                    zframe_t *capnp_msg_body = zmsg_pop(msg_response);
                    size_t size = zframe_size(capnp_msg_body);
                    unsigned char *data = zframe_data(capnp_msg_body);

                    try {
                        auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word *>(data),
                                                       size / sizeof(capnp::word));

                        capnp::FlatArrayMessageReader reader(capnp_data);
                        auto msg_dht_update = reader.getRoot<riaps::discovery::DhtUpdate>();

                        // If update
                        if (msg_dht_update.isProviderUpdate()) {
                            riaps::discovery::ProviderListUpdate::Reader msg_provider_update = msg_dht_update.getProviderUpdate();
                            HandleDhtUpdate(msg_provider_update, client_subscriptions_);

                        } else if (msg_dht_update.isProviderGet()) {
                            riaps::discovery::ProviderListGet::Reader msg_provider_get = msg_dht_update.getProviderGet();
                            HandleDhtGet(msg_provider_get, clients_);
                        } else if (msg_dht_update.isZombieList()) {
                            auto zombie_list = msg_dht_update.getZombieList();
                            for (int i = 0; i < zombie_list.size(); i++) {
                                string current_zombie = zombie_list[i];
                                zombie_services_[current_zombie] = zclock_mono();
                            }
                        } else if (msg_dht_update.isGroupUpdate()) {
                            riaps::discovery::GroupUpdate::Reader msg_group_update = msg_dht_update.getGroupUpdate();
                            HandleDhtGroupUpdate(msg_group_update);
                        }

                        zframe_destroy(&capnp_msg_body);
                        zmsg_destroy(&msg_response);
                    }
                    catch (kj::Exception &e) {
                        logger_->error("Couldn't deserialize message from DHT_ROUTER_SOCKET");
                        continue;
                    }
                }

                    // Handling messages from the RIAPS FW
                else if (which == riaps_socket_) {
                    HandleRiapsMessage();
                } else {
                    //auto outdateds = maintain_servicecache(service_checkins);
                }
            }
        }

        void DiscoveryMessageHandler::HandleRiapsMessage() {
            zmsg_t *riaps_message = zmsg_recv(riaps_socket_);
            if (!riaps_message) {
                logger_->critical("Empty message arrived => interrupted");
                terminated_ = true;
            } else {
                zframe_t *fr = zmsg_pop(riaps_message);
                rep_identity_.reset();
                rep_identity_ = shared_ptr<zframe_t>(fr, [this](zframe_t *oldFrame) {
                    if (oldFrame != nullptr) {
                        zframe_destroy(&oldFrame);
                    }
                });
                zframe_t *empty_frame = zmsg_pop(riaps_message);
                zframe_destroy(&empty_frame);

                zframe_t *capnp_msgbody = zmsg_pop(riaps_message);
                size_t size = zframe_size(capnp_msgbody);
                unsigned char *data = zframe_data(capnp_msgbody);

                riaps::discovery::DiscoReq::Reader msg_disco_req;

                // Convert ZMQ bytes to CAPNP buffer
                auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word *>(data), size / sizeof(capnp::word));

                capnp::FlatArrayMessageReader reader(capnp_data);
                msg_disco_req = reader.getRoot<riaps::discovery::DiscoReq>();

                if (msg_disco_req.isActorReg()) {
                    DBGLN
                    riaps::discovery::ActorRegReq::Reader msg_actor_req = msg_disco_req.getActorReg();
                    HandleActorReg(msg_actor_req);
                } else if (msg_disco_req.isActorUnreg()) {
                    DBGLN
                    riaps::discovery::ActorUnregReq::Reader msg_actor_unreg = msg_disco_req.getActorUnreg();
                    HandleActorUnreg(msg_actor_unreg);
                } else if (msg_disco_req.isServiceReg()) {
                    DBGLN
                    riaps::discovery::ServiceRegReq::Reader msg_service_reg = msg_disco_req.getServiceReg();
                    HandleServiceReg(msg_service_reg);
                } else if (msg_disco_req.isServiceLookup()) {
                    DBGLN
                    riaps::discovery::ServiceLookupReq::Reader msg_service_lookup = msg_disco_req.getServiceLookup();
                    HandleServiceLookup(msg_service_lookup);
                } else if (msg_disco_req.isGroupJoin()) {
                    DBGLN
                    riaps::discovery::GroupJoinReq::Reader msg_group_join = msg_disco_req.getGroupJoin();
                    HandleGroupJoin(msg_group_join);
                }
                zmsg_destroy(&riaps_message);
                zframe_destroy(&capnp_msgbody);
            }
        }

        void DiscoveryMessageHandler::HandleActorReg(riaps::discovery::ActorRegReq::Reader &msg_actor_req) {
            logger_->debug("{}", __FUNCTION__);
            string actorname = string(msg_actor_req.getActorName().cStr());
            string appname = string(msg_actor_req.getAppName().cStr());
            string clientkey_base = fmt::format("/{}/{}/", appname, actorname);
            logger_->info("Register actor with PID - {} : {}", msg_actor_req.getPid(), clientkey_base);

            auto registered_actor_it = clients_.find(clientkey_base);
            bool is_registered = registered_actor_it != clients_.end();

            // If the name of the actor is registered previously and it still run => Error
            bool is_running = is_registered && kill(registered_actor_it->second->pid, 0) == 0;

            // If the actor already registered and running
            if (is_running) {
                logger_->error("Cannot register actor. This actor already registered ({})", clientkey_base);

                capnp::MallocMessageBuilder message;
                auto drepmsg = message.initRoot<riaps::discovery::DiscoRep>();
                auto arepmsg = drepmsg.initActorReg();
                arepmsg.setPort(0);
                arepmsg.setStatus(riaps::discovery::Status::ERR);

                auto serializedMessage = capnp::messageToFlatArray(message);

                zmsg_t *msg = zmsg_new();
                zframe_t *frIdentity = zframe_dup(rep_identity_.get());
                zmsg_add(msg, frIdentity);
                zmsg_addstr(msg, "");
                zmsg_addmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());
                zmsg_send(&msg, riaps_socket_);

            } else {

                // Purge the old instance
                if (is_registered && !is_running) {
                    DeregisterActor(appname, actorname);
                }

                // Open a new PAIR socket for actor communication
                zsock_t *actor_socket = zsock_new (ZMQ_PAIR);
                auto port = zsock_bind(actor_socket, "tcp://*:!");

                //_clients[clientkey_base] = unique_ptr<actor_details_t>(new actor_details_t());
                clients_[clientkey_base] = make_shared<ActorDetails>();
                clients_[clientkey_base]->socket = actor_socket;
                clients_[clientkey_base]->port = port;
                clients_[clientkey_base]->pid = msg_actor_req.getPid();
                clients_[clientkey_base]->app_name = appname;

                // Subscribe to groups
                if (group_listeners_.find(appname) == group_listeners_.end()) {
                    string key = fmt::format("/groups/{}", appname);
                    group_listeners_[appname] =
                            dht_node_.listen<DhtData>(dht::InfoHash(key), [this](vector<DhtData> &&values) {
                                if (values.size() == 0) return true;

                                async(launch::async, &DiscoveryMessageHandler::PushDhtValuesToDisco, this, values);
                                return true;
                            });
                }

                // Create and send the Response
                capnp::MallocMessageBuilder message;
                auto drepmsg = message.initRoot<riaps::discovery::DiscoRep>();
                auto arepmsg = drepmsg.initActorReg();

                arepmsg.setPort(port);
                arepmsg.setStatus(riaps::discovery::Status::OK);

                auto serialized_message = capnp::messageToFlatArray(message);

                zmsg_t *msg = zmsg_new();
                zframe_t *frm_identity = zframe_dup(rep_identity_.get());
                zmsg_add(msg, frm_identity);
                zmsg_addstr(msg, "");
                zmsg_addmem(msg, serialized_message.asBytes().begin(), serialized_message.asBytes().size());

                zmsg_send(&msg, riaps_socket_);

                // Use the same object, do not create another copy of actor_details;
                string clientkey_Local = clientkey_base + mac_address_;
                clients_[clientkey_Local] = clients_[clientkey_base];

                string clientkey_global = clientkey_base + host_address_;
                clients_[clientkey_global] = clients_[clientkey_base];
            }
        }

        void DiscoveryMessageHandler::HandleActorUnreg(riaps::discovery::ActorUnregReq::Reader &msg_actor_unreg) {
            const string actorname = string(msg_actor_unreg.getActorName().cStr());
            const string appname = string(msg_actor_unreg.getAppName().cStr());
            const int servicePid = msg_actor_unreg.getPid();

            // Mark actor's services as zombie
            if (service_checkins_.find(servicePid) != service_checkins_.end()) {
                for (auto &service : service_checkins_[servicePid]) {
                    string service_address = service->value;

                    vector<uint8_t> opendht_data(service_address.begin(), service_address.end());

                    if (service_address.find("127.0.0.1") != string::npos) {
                        DhtPut(zombielocalkey_, opendht_data);
                    } else {
                        DhtPut(zombieglobalkey_, opendht_data);
                    }
                    //m_dhtNode.put(m_zombieKey, dht::Value(opendht_data));

                    // Remove the service from the local db
                    try {
                        auto db = Lmdb::db();
                        db->Del(service->key);
                    } catch (lmdb_error &e) {
                        logger_->error("{}", e.what());
                    }
                }
            }

            int port = DeregisterActor(appname, actorname);

            // Create and send the Response
            capnp::MallocMessageBuilder message;
            auto drepmsg = message.initRoot<riaps::discovery::DiscoRep>();
            auto unregrepmsg = drepmsg.initActorUnreg();

            // If the socket was found
            if (port != -1) {
                unregrepmsg.setStatus(riaps::discovery::Status::OK);
                unregrepmsg.setPort(port);
            } else {
                unregrepmsg.setStatus(riaps::discovery::Status::ERR);
            }

            auto serialized_message = capnp::messageToFlatArray(message);

            zmsg_t *msg = zmsg_new();
            zframe_t *frm_identity = zframe_dup(rep_identity_.get());
            zmsg_add(msg, frm_identity);
            zmsg_addstr(msg, "");
            zmsg_addmem(msg, serialized_message.asBytes().begin(), serialized_message.asBytes().size());

            zmsg_send(&msg, riaps_socket_);
        }

        void DiscoveryMessageHandler::HandleServiceReg(riaps::discovery::ServiceRegReq::Reader &msg_service_reg) {
            logger_->debug("{}", __FUNCTION__);
            auto check_dht = WaitForDht();

            auto msg_path = msg_service_reg.getPath();
            auto msg_sock = msg_service_reg.getSocket();
            auto service_pid = msg_service_reg.getPid();


            auto kv_pair = BuildInsertKeyValuePair(msg_path.getAppName(),
                                                   msg_path.getMsgType(),
                                                   msg_path.getKind(),
                                                   msg_path.getScope(),
                                                   msg_sock.getHost(),
                                                   msg_sock.getPort());

            logger_->info("Register service: {}@{}:{}", get<0>(kv_pair), msg_sock.getHost().cStr(), msg_sock.getPort());

            // Save response port addresses into local database
            // If the discovery service restarts because of failure, then it puts the response ports into zombie state.
            // Previously registered ports don't work after restart.
            if (msg_service_reg.getPath().getKind() == riaps::discovery::Kind::REP) {
                try {
                    auto db = Lmdb::db();
                    db->Put(get<0>(kv_pair), get<1>(kv_pair));
                } catch (lmdb_error &e) {
                    logger_->error("{}", e.what());
                }
            }

            // New pid
            if (service_checkins_.find(service_pid) == service_checkins_.end()) {
                service_checkins_[service_pid] = vector<unique_ptr<ServiceCheckins>>();
            }

            // Add PID - Service Details
            unique_ptr<ServiceCheckins> new_item = unique_ptr<ServiceCheckins>(new ServiceCheckins());
            new_item->createdTime = zclock_mono();
            new_item->key = get<0>(kv_pair);
            new_item->value = get<1>(kv_pair);
            //newItem->pid         = service_pid;
            service_checkins_[service_pid].push_back(move(new_item));

            // Convert the value to bytes
            vector<uint8_t> opendht_data(get<1>(kv_pair).begin(), get<1>(kv_pair).end());
            auto keyhash = dht::InfoHash::get(get<0>(kv_pair));

            check_dht.wait();
            if (!check_dht.get()) {
                logger_->error("DHT may be not ready");
            }

            DhtPut(keyhash, opendht_data);
            zclock_sleep(500);

            //Send response
            capnp::MallocMessageBuilder message;
            auto msg_discorep = message.initRoot<riaps::discovery::DiscoRep>();
            auto msg_servicereg_rep = msg_discorep.initServiceReg();

            msg_servicereg_rep.setStatus(riaps::discovery::Status::OK);

            auto serialized_message = capnp::messageToFlatArray(message);

            zmsg_t *msg = zmsg_new();
            zframe_t *frm_identity = zframe_dup(rep_identity_.get());
            zmsg_add(msg, frm_identity);
            zmsg_addstr(msg, "");
            zmsg_addmem(msg, serialized_message.asBytes().begin(), serialized_message.asBytes().size());

            zmsg_send(&msg, riaps_socket_);


        }

        void DiscoveryMessageHandler::DhtPut(const string &key, vector<uint8_t> &data) {
            auto key_hash = dht::InfoHash::get(key);
            DhtPut(key_hash, data);
        }

        void DiscoveryMessageHandler::DhtPut(dht::InfoHash &keyhash,
                                             vector<uint8_t> &data) {
            DhtData d;
            if (has_security_) {
                d.EncryptData(data, private_key_);
            } else {
                d.raw_data = data;
            }

            dht_node_.put(keyhash, d);
        }

        void
        DiscoveryMessageHandler::HandleServiceLookup(riaps::discovery::ServiceLookupReq::Reader &msg_service_lookup) {
            auto client = msg_service_lookup.getClient();
            auto path = msg_service_lookup.getPath();

            // Key   -> /appname/msgType/kind
            // Value -> /appname/clientactorname/clienthost/clientinstancename/clientportname
            // The "value" is interested in "key"
            auto lookupkey =
                    BuildLookupKey(path.getAppName(),
                                   path.getMsgType(),
                                   path.getKind(),
                                   path.getScope(),
                                   client.getActorHost(),
                                   client.getActorName(),
                                   client.getInstanceName(),
                                   client.getPortName());

            // This client is interested in this kind of messages. Register it.
            auto current_client = make_unique<ClientDetails>();
            current_client->app_name = path.getAppName();
            current_client->actor_host = riaps::framework::Network::GetIPAddress(); //client.getActorHost();
            current_client->port_name = client.getPortName();
            current_client->actor_name = client.getActorName();
            current_client->instance_name = client.getInstanceName();
            current_client->is_local = path.getScope() == riaps::discovery::Scope::LOCAL ? true : false;
            // Copy for the get callback
            ClientDetails currentClientTmp(*current_client);

            // Now using just the discovery service to register the interested clients
            if (client_subscriptions_.find(lookupkey.first) == client_subscriptions_.end()) {
                // Nobody subscribed to this messagetype
                client_subscriptions_[lookupkey.first] = vector<unique_ptr<ClientDetails>>();
            }
            if (find(client_subscriptions_[lookupkey.first].begin(),
                          client_subscriptions_[lookupkey.first].end(),
                          current_client) == client_subscriptions_[lookupkey.first].end()) {

                client_subscriptions_[lookupkey.first].push_back(move(current_client));
            }

            dht::InfoHash lookupkeyhash = dht::InfoHash::get(lookupkey.first);
            logger_->info("Lookup: {}", lookupkey.first);

            //Send response
            capnp::MallocMessageBuilder message;
            auto msg_discorep = message.initRoot<riaps::discovery::DiscoRep>();
            auto msg_service_lookup_rep = msg_discorep.initServiceLookup();
            msg_service_lookup_rep.setStatus(riaps::discovery::Status::OK);

            auto number_of_clients = 0;
            auto sockets = msg_service_lookup_rep.initSockets(number_of_clients);
            auto serialized_message = capnp::messageToFlatArray(message);

            zmsg_t *msg = zmsg_new();
            zframe_t *frm_identity = zframe_dup(rep_identity_.get());
            zmsg_add(msg, frm_identity);
            zmsg_addstr(msg, "");
            zmsg_addmem(msg, serialized_message.asBytes().begin(), serialized_message.asBytes().size());
            zmsg_send(&msg, riaps_socket_);

            // Start listener on this key if it hasn't been started yet.
            if (registered_listeners_.find(lookupkeyhash.toString()) == registered_listeners_.end()) {

                // Add listener to the added key
                auto logger = logger_;
                auto pk = private_key_;
                auto has_security = has_security_;
                registered_listeners_[lookupkeyhash.toString()] =
                        dht_node_.listen<DhtData>(lookupkeyhash,
                                                  [lookupkey, logger, has_security, pk](vector<DhtData> &&values) {
                                                      async(launch::async, [pk, has_security, logger, lookupkey](
                                                              vector<DhtData> values) {
                                                          vector<string> update_results;
                                                          for (DhtData &value :values) {
                                                              if (has_security) {
                                                                  if (!value.DecryptData(pk)) continue;
                                                              }

                                                              string result = string(
                                                                      value.raw_data.begin(),
                                                                      value.raw_data.end());
                                                              logger->debug("OpenDHT.Listen() returns: {}", result);
                                                              update_results.push_back(result);

                                                              zsock_t *notify_ractor_socket = zsock_new_push(
                                                                      DHT_RESULT_CHANNEL);

                                                              capnp::MallocMessageBuilder message;

                                                              auto msg_providerlist_push = message.initRoot<riaps::discovery::DhtUpdate>();
                                                              auto msg_provider_update = msg_providerlist_push.initProviderUpdate();
                                                              msg_provider_update.setProviderpath(lookupkey.first);

                                                              auto number_of_providers = update_results.size();
                                                              ::capnp::List<::capnp::Text>::Builder msg_providers = msg_provider_update.initNewvalues(
                                                                      number_of_providers);

                                                              int provider_index = 0;
                                                              for (string provider : update_results) {
                                                                  ::capnp::Text::Builder b(
                                                                          (char *) provider.c_str());
                                                                  msg_providers.set(
                                                                          provider_index++,
                                                                          b.asString());
                                                              }

                                                              auto serialized_message = capnp::messageToFlatArray(
                                                                      message);

                                                              zmsg_t *msg = zmsg_new();
                                                              zmsg_pushmem(msg,
                                                                           serialized_message.asBytes().begin(),
                                                                           serialized_message.asBytes().size());

                                                              zmsg_send(&msg,
                                                                        notify_ractor_socket);

                                                              sleep(1);
                                                              zsock_destroy(&notify_ractor_socket);
                                                              sleep(1);
                                                          }
                                                      }, values);


                                                      return true; // keep listening
                                                  }
                        );
            }

            zclock_sleep(400);

            DhtGet(lookupkey.first, currentClientTmp, 0);
        }

        void DiscoveryMessageHandler::DhtGet(const string lookup_key, ClientDetails client_details, uint8_t call_level) {
            auto logger = logger_;
            auto pk = private_key_;
            auto has_security = has_security_;
            auto data_id = dht::InfoHash::get(lookup_key);
            dht_node_.get<DhtData>(data_id,
                                   [client_details, logger, lookup_key, has_security, pk]
                                           (DhtData &&value) {

                                       async(launch::async, [pk, has_security, client_details](DhtData d) {
                                           if (has_security) {
                                               if (!d.DecryptData(pk)) return;
                                           }
                                           string result = string(d.raw_data.begin(), d.raw_data.end());

                                           zsock_t *notify_ractor_socket = zsock_new_push(DHT_RESULT_CHANNEL);

                                           capnp::MallocMessageBuilder message;
                                           auto msg_providerlistpush = message.initRoot<riaps::discovery::DhtUpdate>();
                                           riaps::discovery::ProviderListGet::Builder msg_providerget = msg_providerlistpush.initProviderGet();

                                           auto msg_path = msg_providerget.initPath();

                                           riaps::discovery::Scope scope = client_details.is_local
                                                                           ? riaps::discovery::Scope::LOCAL
                                                                           : riaps::discovery::Scope::GLOBAL;
                                           string app_name = client_details.app_name;

                                           msg_path.setScope(scope);
                                           msg_path.setAppName(client_details.app_name);

                                           auto msg_client = msg_providerget.initClient();

                                           msg_client.setActorName(client_details.actor_name);
                                           msg_client.setPortName(client_details.port_name);
                                           msg_client.setActorHost(client_details.actor_host);
                                           msg_client.setInstanceName(client_details.instance_name);

                                           auto msg_get_results = msg_providerget.initResults(1);
                                           msg_get_results.set(0, result);

                                           auto serialized_message = capnp::messageToFlatArray(message);

                                           zmsg_t *msg = zmsg_new();
                                           auto bytes = serialized_message.asBytes();
                                           zmsg_pushmem(msg, bytes.begin(), bytes.size());
                                           zmsg_send(&msg, notify_ractor_socket);

                                           sleep(1);
                                           zsock_destroy(&notify_ractor_socket);
                                           sleep(1);

                                       }, value);

                                       return true;
                                   }, [logger, lookup_key, client_details, call_level, this](bool success) {
                        logger->debug("OpenDHT.Get({}) finished with '{}'  callback done!", lookup_key, success);
                        if (!success) {
                            if (call_level < 10) {
                                thread t([lookup_key, client_details, call_level, this]() {
                                    zclock_sleep(1000);
                                    this->DhtGet(lookup_key, client_details, call_level + 1);
                                });
                                t.detach();
                            }
                        }

                    }
            );
        }

        void DiscoveryMessageHandler::HandleGroupJoin(riaps::discovery::GroupJoinReq::Reader &msg_group_join) {

            // Join to the group.
            auto msg_groupservices = msg_group_join.getServices();
            string appname = msg_group_join.getAppName();
            string component_id = msg_group_join.getComponentId();
            auto actor_pid = msg_group_join.getPid();

            riaps::groups::GroupDetails group_details;
            group_details.app_name = appname;
            group_details.component_id = component_id;
            group_details.group_id = {
                    msg_group_join.getGroupId().getGroupType(),
                    msg_group_join.getGroupId().getGroupName()
            };

            for (int i = 0; i < msg_groupservices.size(); i++) {
                group_details.group_services.push_back({
                                                              msg_groupservices[i].getMessageType(),
                                                              msg_groupservices[i].getAddress()
                                                      });
                logger_->debug("REG: {}", msg_groupservices[i].getAddress().cStr());
            }

            string key = fmt::format("/groups/{}", appname);

            //Send response
            capnp::MallocMessageBuilder rep_message;
            auto msg_discorep = rep_message.initRoot<riaps::discovery::DiscoRep>();
            auto msg_group_join_rep = msg_discorep.initGroupJoin();

            msg_group_join_rep.setStatus(riaps::discovery::Status::OK);

            auto serialized_message = capnp::messageToFlatArray(rep_message);

            zmsg_t *msg = zmsg_new();
            zframe_t *frm_identity = zframe_dup(rep_identity_.get());
            zmsg_add(msg, frm_identity);
            zmsg_addstr(msg, "");
            zmsg_addmem(msg, serialized_message.asBytes().begin(), serialized_message.asBytes().size());

            zmsg_send(&msg, riaps_socket_);

            dht_node_.get<DhtData>(dht::InfoHash(key), [this](vector<DhtData> &&values) {
                if (values.size() == 0) return true;

                async(launch::async, &DiscoveryMessageHandler::PushDhtValuesToDisco, this, values);

                return true;
            });

            zclock_sleep(1000);

            /**
             * Store the details for renewing
             */
            if (group_services_.find(actor_pid) == group_services_.end()) {
                group_services_[actor_pid] == vector<shared_ptr<RegisteredGroup>>();
            }
            auto current_groupreg = make_shared<RegisteredGroup>(RegisteredGroup{
                    key,
                    group_details,
                    actor_pid,
                    Timeout<chrono::minutes>(10) //10 minutes
            });
            group_services_[actor_pid].push_back(move(current_groupreg));

            auto group_details_blob = dht::packMsg<riaps::groups::GroupDetails>(group_details);
            DhtPut(key, group_details_blob);
        }

        void DiscoveryMessageHandler::PushDhtValuesToDisco(vector<DhtData> &&values) {
            zsock_t *dht_notification_socket = zsock_new_push(DHT_RESULT_CHANNEL);
            zsock_set_linger(dht_notification_socket, 0);
            zsock_set_sndtimeo(dht_notification_socket, 0);


            // Let's unpack the data
            for (auto &value : values) {
                if (has_security_) {
                    if (!value.DecryptData(private_key_))
                        continue;
                }

                riaps::groups::GroupDetails v = dht::unpackMsg<riaps::groups::GroupDetails>(value.raw_data);

                capnp::MallocMessageBuilder dht_message;
                auto msg_dht_update = dht_message.initRoot<riaps::discovery::DhtUpdate>();
                auto msg_group_update = msg_dht_update.initGroupUpdate();
                msg_group_update.setComponentId(v.component_id);
                msg_group_update.setAppName(v.app_name);

                auto group_id = msg_group_update.initGroupId();
                group_id.setGroupName(v.group_id.group_name);
                group_id.setGroupType(v.group_id.group_type_id);

                auto group_services = msg_group_update.initServices(v.group_services.size());
                for (int i = 0; i < v.group_services.size(); i++) {
                    auto& gs = v.group_services[i];
                    auto address = fmt::format("{}:{}", gs.host, gs.port);
                    group_services[i].setAddress(address);
                    group_services[i].setMessageType(gs.message_type);
                }

                auto serialized_message = capnp::messageToFlatArray(dht_message);

                zmsg_t *msg = zmsg_new();
                auto bytes = serialized_message.asBytes();
                zmsg_pushmem(msg, bytes.begin(), bytes.size());
                zmsg_send(&msg, dht_notification_socket);
                //cout << "[DHT] Group notifications sent to discovery service" << endl;
            }

            zclock_sleep(100);
            zsock_destroy(&dht_notification_socket);
        }

        // Handle ZMQ messages, arriving on the zactor PIPE
        void DiscoveryMessageHandler::HandlePipeMessage() {
            zmsg_t *msg = zmsg_recv(pipe_);
            if (!msg) {
                cout << "No msg => interrupted" << endl;
                terminated_ = true;
            } else {
                char *command = zmsg_popstr(msg);

                if (streq(command, "$TERM")) {
                    logger_->info("$TERMINATE arrived, discovery service is stopping.");
                    terminated_ = true;
                } else if (streq(command, CMD_DISCO_JOIN)) {
                    bool has_more_msg = true;

                    while (has_more_msg) {
                        char *newhost = zmsg_popstr(msg);
                        if (newhost) {
                            logger_->info("Join: {}", newhost);
                            string str_newhost(newhost);
                            dht_node_.bootstrap(str_newhost, to_string(RIAPS_DHT_NODE_PORT));
                            zstr_free(&newhost);
                        } else {
                            has_more_msg = false;
                        }
                    }
                }

                if (command) {
                    zstr_free(&command);
                }
                zmsg_destroy(&msg);
            }
        }

        bool DiscoveryMessageHandler::HandleZombieUpdate(vector<DhtData> &&values) {
            vector<string> dht_results;

            for (auto &value :values) {
                if (has_security_) {
                    if (!value.DecryptData(private_key_))
                        continue;
                }
                string result = string(value.raw_data.begin(), value.raw_data.end());
                dht_results.push_back(result);
            }

            if (dht_results.size() > 0) {
                zsock_t *dht_notification = zsock_new_push(DHT_RESULT_CHANNEL);
                capnp::MallocMessageBuilder message;

                auto msg_dht_update = message.initRoot<riaps::discovery::DhtUpdate>();
                auto msg_zombie_list = msg_dht_update.initZombieList(dht_results.size());

                for (int i = 0; i < dht_results.size(); i++) {
                    char *cres = (char *) dht_results[i].c_str();
                    msg_zombie_list.set(i, capnp::Text::Builder(cres));
                    logger_->info("Service is considered as zombie: {}", dht_results[i].c_str());
                }

                auto serialized_message = capnp::messageToFlatArray(message);

                zmsg_t *msg = zmsg_new();
                zmsg_pushmem(msg, serialized_message.asBytes().begin(), serialized_message.asBytes().size());

                zmsg_send(&msg, dht_notification);
                zclock_sleep(500);
                zsock_destroy(&dht_notification);
            }
            return true;
        }

        void DiscoveryMessageHandler::HandleDhtGet(
                const riaps::discovery::ProviderListGet::Reader &msg_provider_get,
                const map<string, shared_ptr<ActorDetails>> &clients) {
            auto msg_get_results = msg_provider_get.getResults();

            for (int idx = 0; idx < msg_get_results.size(); idx++) {
                string result_endpoint = msg_get_results[idx].cStr();

                if (zombie_services_.find(result_endpoint) != zombie_services_.end()) continue;

                auto pos = result_endpoint.find(':');
                if (pos == string::npos) {
                    continue;
                }

                string host = result_endpoint.substr(0, pos);
                string port = result_endpoint.substr(pos + 1, string::npos);
                int port_num = -1;

                try {
                    port_num = stoi(port);
                } catch (invalid_argument &e) {
                    logger_->error("Cast error, string -> int, portnumber: {}\n{}", port, e.what());
                    continue;
                }
                catch (out_of_range &e) {
                    logger_->error("Cast error, string -> int, portnumber: {}\n{}", port, e.what());
                    continue;
                }

                capnp::MallocMessageBuilder message;
                auto msg_discoupd = message.initRoot<riaps::discovery::DiscoUpd>();
                auto msg_portupd = msg_discoupd.initPortUpdate();
                auto msg_client = msg_portupd.initClient();
                auto msg_socket = msg_portupd.initSocket();

                // Set up client
                msg_client.setActorHost(msg_provider_get.getClient().getActorHost());
                msg_client.setActorName(msg_provider_get.getClient().getActorName());
                msg_client.setInstanceName(msg_provider_get.getClient().getInstanceName());
                msg_client.setPortName(msg_provider_get.getClient().getPortName());

                msg_portupd.setScope(msg_provider_get.getPath().getScope());

                msg_socket.setHost(host);
                msg_socket.setPort(port_num);

                auto serialized_message = capnp::messageToFlatArray(message);

                zmsg_t *msg = zmsg_new();
                zmsg_pushmem(msg, serialized_message.asBytes().begin(),
                             serialized_message.asBytes().size());

                string client_key_base = fmt::format("/{}/{}/",
                                                   msg_provider_get.getPath().getAppName().cStr(),
                                                   msg_provider_get.getClient().getActorName().cStr());

                // Client might gone while the DHT was looking for the values
                if (clients.find(client_key_base) != clients.end()) {
                    zmsg_send(&msg, clients.at(client_key_base)->socket);
                    logger_->info("Get() returns {}@{}:{} to {}",
                                  msg_provider_get.getClient().getPortName().cStr(),
                                  host,
                                  port_num,
                                  client_key_base);

                } else {
                    zmsg_destroy(&msg);
                    logger_->warn("Get returned with values, but client has gone.");
                }
            }
        }

        void DiscoveryMessageHandler::HandleDhtUpdate(const riaps::discovery::ProviderListUpdate::Reader &msg_provider_update,
                                                      const map<string, vector<unique_ptr<ClientDetails>>> &client_subscriptions) {

            string provider_key = string(msg_provider_update.getProviderpath().cStr());

            auto msg_newproviders = msg_provider_update.getNewvalues();

            // Look for services who may interested in the new provider
            if (client_subscriptions.find(provider_key) != client_subscriptions.end()) {
                for (auto &subscribed_client : client_subscriptions.at(provider_key)) {
                    for (int idx = 0; idx < msg_newproviders.size(); idx++) {
                        string new_provider_endpoint = string(msg_newproviders[idx].cStr());

                        // If the service marked as zombie
                        if (zombie_services_.find(new_provider_endpoint) != zombie_services_.end()) continue;

                        auto pos = new_provider_endpoint.find(':');
                        if (pos == string::npos) {
                            continue;
                        }

                        string host = new_provider_endpoint.substr(0, pos);
                        string port = new_provider_endpoint.substr(pos + 1, string::npos);
                        int portNum = -1;

                        try {
                            portNum = stoi(port);
                        } catch (invalid_argument &e) {
                            cout << "Cast error, string -> int, portnumber: " << port << endl;
                            cout << e.what() << endl;
                            continue;
                        }
                        catch (out_of_range &e) {
                            cout << "Cast error, string -> int, portnumber: " << port << endl;
                            cout << e.what() << endl;
                            continue;
                        }

                        string clientkey_base = fmt::format("/{}/{}/",
                                                           subscribed_client->app_name,
                                                           subscribed_client->actor_name);

                        logger_->info("Search for registered actor: {}", clientkey_base);

                        // If the client port saved before
                        if (clients_.find(clientkey_base) != clients_.end()) {
                            const ActorDetails *clientSocket = clients_.at(clientkey_base).get();

                            if (clientSocket->socket != NULL) {
                                capnp::MallocMessageBuilder message;
                                auto msg_discoupd = message.initRoot<riaps::discovery::DiscoUpd>();
                                auto msg_portupd = msg_discoupd.initPortUpdate();
                                auto msg_client = msg_portupd.initClient();
                                auto msg_socket = msg_portupd.initSocket();

                                // Set up client
                                msg_client.setActorHost(subscribed_client->actor_host);
                                msg_client.setActorName(subscribed_client->actor_name);
                                msg_client.setInstanceName(subscribed_client->instance_name);
                                msg_client.setPortName(subscribed_client->port_name);

                                msg_portupd.setScope(subscribed_client->is_local ? riaps::discovery::Scope::LOCAL
                                                                               : riaps::discovery::Scope::GLOBAL);

                                msg_socket.setHost(host);
                                msg_socket.setPort(portNum);

                                auto serialized_message = capnp::messageToFlatArray(message);

                                zmsg_t *msg = zmsg_new();
                                zmsg_pushmem(msg, serialized_message.asBytes().begin(),
                                             serialized_message.asBytes().size());

                                zmsg_send(&msg, clientSocket->socket);

                                logger_->info("Update() returns {}@{}:{} to {}", subscribed_client->port_name, host,
                                              portNum, clientkey_base);
                            }
                        }
                    }
                }
            }
        }

        void DiscoveryMessageHandler::HandleDhtGroupUpdate(const riaps::discovery::GroupUpdate::Reader &msg_group_update) {
            // Look for the affected actors
            string app_name = msg_group_update.getAppName().cStr();
            bool actor_found = false;

            set<zsock_t *> sent_cache;

            for (auto &client : clients_) {
                if (client.second->app_name == app_name) {
                    if (sent_cache.find(client.second->socket) != sent_cache.end()) continue;

                    string log;
                    for (int i = 0; i < msg_group_update.getServices().size(); i++) {

                        log += msg_group_update.getServices()[i].getAddress().cStr();
                        log += "; ";
                    }
                    logger_->debug("UPD: {}", log);

                    // Store the socket pointer to avoid multiple sending of the same update.
                    sent_cache.insert(client.second->socket);
                    actor_found == true;

                    capnp::MallocMessageBuilder builder;
                    auto msg_disco_update = builder.initRoot<riaps::discovery::DiscoUpd>();
                    msg_disco_update.setGroupUpdate(msg_group_update);

                    auto serialized_message = capnp::messageToFlatArray(builder);

                    zmsg_t *msg = zmsg_new();
                    zmsg_pushmem(msg, serialized_message.asBytes().begin(), serialized_message.asBytes().size());

                    zmsg_send(&msg, client.second.get()->socket);
                }
            }

            // TODO: No active actor for this app, purge this listener
            if (!actor_found) {

            }
        }


        void DiscoveryMessageHandler::RenewServices() {
            int64_t now = zclock_mono();
            for (auto pid_it = service_checkins_.begin(); pid_it != service_checkins_.end(); pid_it++) {
                for (auto service_it = pid_it->second.begin(); service_it != pid_it->second.end(); service_it++) {

                    // Renew
                    if (now - (*service_it)->createdTime > (*service_it)->timeout) {
                        (*service_it)->createdTime = now;

                        // Reput key-value
                        vector<uint8_t> opendht_data((*service_it)->value.begin(), (*service_it)->value.end());
                        auto keyhash = dht::InfoHash::get((*service_it)->key);
                        this->DhtPut(keyhash, opendht_data);
                    }
                }
            }
        }


        void DiscoveryMessageHandler::MaintainZombieList() {
            int64_t currentTime = zclock_mono();

            auto it = zombie_services_.begin();
            while (it != zombie_services_.end()) {
                // Purge zombies after 15 minutes
                if ((currentTime - it->second) > 60 * 15 * 1000) {
                    logger_->info("Purge zombie from cache: {}", it->first);
                    it = zombie_services_.erase(it);
                } else {
                    ++it;
                }
            }
        }

        int DiscoveryMessageHandler::DeregisterActor(const string &app_name,
                                                     const string &actor_name) {

            string clientkey_base = fmt::format("/{}/{}/", app_name, actor_name);
            string clientkey_local = clientkey_base + mac_address_;
            string clientkey_global = clientkey_base + host_address_;

            vector<string> erase_keys {clientkey_base, clientkey_local, clientkey_global};

            logger_->info("Unregister actor: {}", clientkey_base);

            int port = -1;
            if (clients_.find(clientkey_base) != clients_.end()) {
                port = clients_[clientkey_base]->port;
            }

            for (auto it = erase_keys.begin(); it != erase_keys.end(); it++) {
                if (clients_.find(*it) != clients_.end()) {

                    // erased elements
                    int erased = clients_.erase(*it);
                    if (erased == 0) {
                        logger_->error("Couldn't find actor to unregister: {}", *it);
                    }
                }
            }

            return port;
        }

        DiscoveryMessageHandler::~DiscoveryMessageHandler() {
            zpoller_destroy(&poller_);
            zsock_destroy(&dht_update_socket_);
            zsock_destroy(&riaps_socket_);
            zactor_destroy(&dht_tracker_);
            zclock_sleep(500);

            for (auto it_client = clients_.begin(); it_client != clients_.end(); it_client++) {
                if (it_client->second->socket != nullptr) {
                    zsock_destroy(&it_client->second->socket);
                    it_client->second->socket = nullptr;
                }
            }
            sleep(1);
        }

        const tuple<const string, const string> DiscoveryMessageHandler::BuildInsertKeyValuePair(
                const string &app_name,
                const string &msg_type,
                const riaps::discovery::Kind &kind,
                const riaps::discovery::Scope &scope,
                const string &host,
                const uint16_t port) {

            string key = fmt::format("/{}/{}/{}", app_name, msg_type, kindMap.at(kind));

            if (scope == riaps::discovery::Scope::LOCAL) {
                string mac_address = riaps::framework::Network::GetMacAddressStripped();
                key += mac_address;
            }

            const string value = fmt::format("{}:{}", host, to_string(port));

            return make_tuple(key, value);
        }

        const pair<const string, const string> DiscoveryMessageHandler::BuildLookupKey(
                const string &app_name,
                const string &msg_type,
                const riaps::discovery::Kind &kind,
                const riaps::discovery::Scope &scope,
                const string &client_actor_host,
                const string &client_actor_name,
                const string &client_instance_name,
                const string &client_port_name) {

            const unordered_map<riaps::discovery::Kind, string> kindPairs = {
                    {riaps::discovery::Kind::CLT , kindMap.at(riaps::discovery::Kind::SRV)},
                    {riaps::discovery::Kind::SUB , kindMap.at(riaps::discovery::Kind::PUB)},
                    {riaps::discovery::Kind::REQ , kindMap.at(riaps::discovery::Kind::REP)},
                    {riaps::discovery::Kind::REP , kindMap.at(riaps::discovery::Kind::REQ)},
                    {riaps::discovery::Kind::QRY , kindMap.at(riaps::discovery::Kind::ANS)},
                    {riaps::discovery::Kind::ANS , kindMap.at(riaps::discovery::Kind::QRY)},
                    {riaps::discovery::Kind::GSUB, kindMap.at(riaps::discovery::Kind::GPUB)}
            };

            string key = fmt::format("/{}/{}/{}", app_name, msg_type, kindPairs.at(kind));
            const string hostid = riaps::framework::Network::GetMacAddressStripped();

            if (scope == riaps::discovery::Scope::LOCAL) {
                key += hostid;
            }

            string client = fmt::format("/{}/{}/{}/{}/{}", app_name, client_actor_name, client_actor_host,
                                        client_instance_name, client_port_name);

            if (scope == riaps::discovery::Scope::LOCAL) {
                client = client + ":" + hostid;
            }

            return {key, client};
        }
    }
}
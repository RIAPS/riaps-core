#include <discoveryd/r_msghandler.h>
#include <framework/rfw_configuration.h>
#include <framework/rfw_network_interfaces.h>
#include <framework/rfw_security.h>
#include <discoveryd/r_msghandler.h>
#include <discoveryd/r_dhttracker.h>
#include <utils/r_lmdb.h>

using namespace std;

namespace riaps{
    DiscoveryMessageHandler::DiscoveryMessageHandler(dht::DhtRunner &dht_node,
                                                     zsock_t** pipe,
                                                     std::shared_ptr<spdlog::logger> logger)
        : dht_node_(dht_node),
          pipe_(*pipe),
          service_check_period_((uint16_t)20000), // 20 sec in in msec.
          zombie_check_period_((uint16_t)600000), // 10 min in msec
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

        last_service_checkin_ = last_zombie_check_ = zclock_mono();

        // Get current global zombies
        dht_node_.get<DhtData>(dht::InfoHash::get(zombieglobalkey_), [this](std::vector<DhtData>&& values){
            async(std::launch::async, &DiscoveryMessageHandler::HandleZombieUpdate, this, values);
            return true;
        });

        // Get local zombies
        dht_node_.get<DhtData>(dht::InfoHash::get(zombielocalkey_), [this](std::vector<DhtData>&& values){
            std::async(std::launch::async, &DiscoveryMessageHandler::HandleZombieUpdate, this, values);
            return true;
        });

        // Subscribe for further zombies
        dht_node_.listen<DhtData>(dht::InfoHash::get(zombieglobalkey_), [this](std::vector<DhtData>&& values){
            std::async(std::launch::async, &DiscoveryMessageHandler::HandleZombieUpdate, this, values);
            return true;
        });

        dht_node_.listen<DhtData>(dht::InfoHash::get(zombielocalkey_), [this](std::vector<DhtData>&& values){
            std::async(std::launch::async, &DiscoveryMessageHandler::HandleZombieUpdate, this, values);
            return true;
        });

        // The discovery service may be restarted, previously registered response ports are put under the zombie key
        try{
            auto db = Lmdb::db();
            auto services = db->GetAll();

            for (auto& service : *services) {
                auto key = get<0>(service);
                auto address = get<1>(service);
                vector<uint8_t> opendht_data(address.begin(), address.end());
                logger_->info("Mark service {}:{} as zombie", key,address);
                if (address.find("127.0.0.1") != string::npos) {
                    DhtPut(zombielocalkey_, opendht_data);
                } else {
                    DhtPut(zombieglobalkey_, opendht_data);
                }
                db->Del(key);
            }
        } catch(lmdb_error& e) {
            logger_->error("{}", e.what());
        }
    }

    future<bool> DiscoveryMessageHandler::WaitForDht() {
        auto& logger = logger_;
        return std::async(std::launch::async, [logger]()->bool{
            zsock_t* query = zsock_new(ZMQ_DEALER);
            zuuid_t* socketId = zuuid_new();
            zsock_set_identity(query, zuuid_str(socketId));
            zsock_connect(query, CHAN_IN_DHTTRACKER);

            // Retry
            auto retry = 10;
            uint8_t result =0;
            while(retry-->0 && !result) {
                zsock_send(query, "ss", CMD_QUERY_STABLE, "void");
                zsock_recv(query, "1", &result);

                if (!result) {
                    logger->info("DHT is not ready.");
                    zclock_sleep(500);
                }
            }
            zuuid_destroy(&socketId);
            zsock_destroy(&query);
            return result==1;
        });
    }

    void DiscoveryMessageHandler::Run() {
        while (!terminated_){
            void *which = zpoller_wait(poller_, REGULAR_MAINTAIN_PERIOD);

            // Check services whether they are still alive.
            // Reregister the too old services (OpenDHT ValueType settings, it is 10 minutes by default)
            int64_t loopStartTime = zclock_mono();
            if ((loopStartTime-last_service_checkin_) > service_check_period_){
                RenewServices();
            }

            // Check outdated zombies
            if ((loopStartTime-last_zombie_check_) > zombie_check_period_) {
                MaintainZombieList();
            }

            // Handling messages from the caller (e.g.: $TERM$)
            if (which == pipe_) {
                HandlePipeMessage();
            }
            else if (which == dht_update_socket_){
                // Process the updated nodes
                zmsg_t* msgResponse = zmsg_recv(dht_update_socket_);

                zframe_t* capnpMsgBody = zmsg_pop(msgResponse);
                size_t    size = zframe_size(capnpMsgBody);
                unsigned char* data = zframe_data(capnpMsgBody);

                try {
                    auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word *>(data), size / sizeof(capnp::word));

                    capnp::FlatArrayMessageReader reader(capnp_data);
                    auto msgDhtUpdate = reader.getRoot<riaps::discovery::DhtUpdate>();

                    // If update
                    if (msgDhtUpdate.isProviderUpdate()) {
                        riaps::discovery::ProviderListUpdate::Reader msgProviderUpdate = msgDhtUpdate.getProviderUpdate();
                        HandleDhtUpdate(msgProviderUpdate, client_subscriptions_);

                    } else if (msgDhtUpdate.isProviderGet()) {
                        riaps::discovery::ProviderListGet::Reader msgProviderGet = msgDhtUpdate.getProviderGet();
                        HandleDhtGet(msgProviderGet, clients_);
                    } else if (msgDhtUpdate.isZombieList()) {
                        auto zombieList = msgDhtUpdate.getZombieList();
                        for (int i =0; i< zombieList.size(); i++){
                            string currentZombie = zombieList[i];
                            zombie_services_[currentZombie] = zclock_mono();
                        }
                    } else if (msgDhtUpdate.isGroupUpdate()){
                        riaps::discovery::GroupUpdate::Reader msgGroupUpdate = msgDhtUpdate.getGroupUpdate();
                        HandleDhtGroupUpdate(msgGroupUpdate);
                    }

                    zframe_destroy(&capnpMsgBody);
                    zmsg_destroy(&msgResponse);
                }
                catch(kj::Exception& e){
                    logger_->error("Couldn't deserialize message from DHT_ROUTER_SOCKET");
                    continue;
                }
            }

                // Handling messages from the RIAPS FW
            else if(which == riaps_socket_){
                HandleRiapsMessage();
            }
            else {
                //auto outdateds = maintain_servicecache(service_checkins);
            }
        }
    }

    void DiscoveryMessageHandler::HandleRiapsMessage() {
        zmsg_t *riapsMessage = zmsg_recv(riaps_socket_);
        if (!riapsMessage) {
            logger_->critical("Empty message arrived => interrupted");
            terminated_ = true;
        } else {
            zframe_t* fr = zmsg_pop(riapsMessage);
            rep_identity_.reset();
            rep_identity_ = std::shared_ptr<zframe_t>(fr, [this](zframe_t* oldFrame){
                if (oldFrame!=nullptr) {
                    zframe_destroy(&oldFrame);
                }
            });
            zframe_t* emptyFrame = zmsg_pop(riapsMessage);
            zframe_destroy(&emptyFrame);

            zframe_t *capnp_msgbody = zmsg_pop(riapsMessage);
            size_t size = zframe_size(capnp_msgbody);
            unsigned char* data = zframe_data(capnp_msgbody);

            riaps::discovery::DiscoReq::Reader msgDiscoReq;

            // Convert ZMQ bytes to CAPNP buffer
            auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word *>(data), size / sizeof(capnp::word));

            capnp::FlatArrayMessageReader reader(capnp_data);
            msgDiscoReq = reader.getRoot<riaps::discovery::DiscoReq>();

            if (msgDiscoReq.isActorReg()) {
                riaps::discovery::ActorRegReq::Reader msgActorReq = msgDiscoReq.getActorReg();
                HandleActorReg(msgActorReq);
            } else if (msgDiscoReq.isActorUnreg()){
                riaps::discovery::ActorUnregReq::Reader msgActorUnreg = msgDiscoReq.getActorUnreg();
                HandleActorUnreg(msgActorUnreg);
            } else if (msgDiscoReq.isServiceReg()){
                riaps::discovery::ServiceRegReq::Reader msgServiceReg = msgDiscoReq.getServiceReg();
                HandleServiceReg(msgServiceReg);
            } else if (msgDiscoReq.isServiceLookup()){
                riaps::discovery::ServiceLookupReq::Reader msgServiceLookup = msgDiscoReq.getServiceLookup();
                HandleServiceLookup(msgServiceLookup);
            } else if (msgDiscoReq.isGroupJoin()){
                riaps::discovery::GroupJoinReq::Reader msgGroupJoin = msgDiscoReq.getGroupJoin();
                HandleGroupJoin(msgGroupJoin);
            }
            zmsg_destroy(&riapsMessage);
            zframe_destroy(&capnp_msgbody);
        }
    }

    void DiscoveryMessageHandler::HandleActorReg(riaps::discovery::ActorRegReq::Reader& msgActorReq) {

        string actorname = string(msgActorReq.getActorName().cStr());
        string appname   = string(msgActorReq.getAppName().cStr());

        string clientKeyBase = fmt::format("/{}/{}/",appname, actorname);
        logger_->info("Register actor with PID - {} : {}", msgActorReq.getPid(), clientKeyBase);

        auto registeredActorIt = clients_.find(clientKeyBase);
        bool isRegistered =  registeredActorIt!= clients_.end();

        // If the name of the actor is registered previously and it still run => Error
        bool isRunning = isRegistered && kill(registeredActorIt->second->pid, 0) == 0;

        // If the actor already registered and running
        if (isRunning) {
            logger_->error("Cannot register actor. This actor already registered ({})", clientKeyBase);

            capnp::MallocMessageBuilder message;
            auto drepmsg = message.initRoot<riaps::discovery::DiscoRep>();
            auto arepmsg = drepmsg.initActorReg();
            arepmsg.setPort(0);
            arepmsg.setStatus(riaps::discovery::Status::ERR);

            auto serializedMessage = capnp::messageToFlatArray(message);

            zmsg_t *msg = zmsg_new();
            zframe_t* frIdentity = zframe_dup(rep_identity_.get());
            zmsg_add(msg, frIdentity);
            zmsg_addstr(msg, "");
            zmsg_addmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());
            zmsg_send(&msg, riaps_socket_);

        } else {

            // Purge the old instance
            if (isRegistered && !isRunning) {
                DeregisterActor(appname, actorname);
            }

            // Open a new PAIR socket for actor communication
            zsock_t *actor_socket = zsock_new (ZMQ_PAIR);
            auto port = zsock_bind(actor_socket, "tcp://*:!");

            //_clients[clientKeyBase] = std::unique_ptr<actor_details_t>(new actor_details_t());
            clients_[clientKeyBase] = std::make_shared<ActorDetails>();
            clients_[clientKeyBase] -> socket   = actor_socket;
            clients_[clientKeyBase] -> port     = port;
            clients_[clientKeyBase] -> pid      = msgActorReq.getPid();
            clients_[clientKeyBase] -> app_name = appname;

            // Subscribe to groups
            if (group_listeners_.find(appname) == group_listeners_.end()) {
                string key = fmt::format("/groups/{}",appname);
                group_listeners_[appname] =
                        dht_node_.listen<DhtData>(dht::InfoHash(key), [this](vector<DhtData>&& values){
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

            auto serializedMessage = capnp::messageToFlatArray(message);

            zmsg_t *msg = zmsg_new();
            zframe_t* frIdentity = zframe_dup(rep_identity_.get());
            zmsg_add(msg, frIdentity);
            zmsg_addstr(msg, "");
            zmsg_addmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

            zmsg_send(&msg, riaps_socket_);

            // Use the same object, do not create another copy of actor_details;
            string clientKeyLocal = clientKeyBase + mac_address_;
            clients_[clientKeyLocal] = clients_[clientKeyBase];

            string clientKeyGlobal = clientKeyBase + host_address_;
            clients_[clientKeyGlobal] = clients_[clientKeyBase];
        }
    }

    void DiscoveryMessageHandler::HandleActorUnreg(riaps::discovery::ActorUnregReq::Reader &msgActorUnreg) {
        const string actorname = string(msgActorUnreg.getActorName().cStr());
        const string appname = string(msgActorUnreg.getAppName().cStr());
        const int servicePid = msgActorUnreg.getPid();

        // Mark actor's services as zombie
        if (service_checkins_.find(servicePid)!=service_checkins_.end()){
            for (auto& service : service_checkins_[servicePid]){
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
                } catch(lmdb_error& e){
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

        auto serializedMessage = capnp::messageToFlatArray(message);

        zmsg_t *msg = zmsg_new();
        zframe_t* frIdentity = zframe_dup(rep_identity_.get());
        zmsg_add(msg, frIdentity);
        zmsg_addstr(msg, "");
        zmsg_addmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

        zmsg_send(&msg, riaps_socket_);
    }

    void DiscoveryMessageHandler::HandleServiceReg(riaps::discovery::ServiceRegReq::Reader &msgServiceReg) {
        auto check_dht = WaitForDht();

        auto msgPath           = msgServiceReg.getPath();
        auto msgSock           = msgServiceReg.getSocket();
        auto servicePid         = msgServiceReg.getPid();


        auto kv_pair = BuildInsertKeyValuePair(msgPath.getAppName(),
                                               msgPath.getMsgType(),
                                               msgPath.getKind(),
                                               msgPath.getScope(),
                                               msgSock.getHost(),
                                               msgSock.getPort());

        logger_->info("Register service: {}@{}:{}", get<0>(kv_pair), msgSock.getHost().cStr(), msgSock.getPort());

        // Save response port addresses into local database
        // If the discovery service restarts because of failure, then it puts the response ports into zombie state.
        // Previously registered ports don't work after restart.
        if (msgServiceReg.getPath().getKind() == riaps::discovery::Kind::REP) {
            try {
                auto db = Lmdb::db();
                db->Put(get<0>(kv_pair), get<1>(kv_pair));
            } catch(lmdb_error& e) {
                logger_->error("{}", e.what());
            }
        }

        // New pid
        if (service_checkins_.find(servicePid) == service_checkins_.end()) {
            service_checkins_[servicePid] = vector<unique_ptr<ServiceCheckins>>();
        }

        // Add PID - Service Details
        unique_ptr<ServiceCheckins> newItem = unique_ptr<ServiceCheckins>(new ServiceCheckins());
        newItem->createdTime = zclock_mono();
        newItem->key         = std::get<0>(kv_pair);
        newItem->value       = std::get<1>(kv_pair);
        //newItem->pid         = servicePid;
        service_checkins_[servicePid].push_back(std::move(newItem));

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

        auto serializedMessage = capnp::messageToFlatArray(message);

        zmsg_t *msg = zmsg_new();
        zframe_t* frIdentity = zframe_dup(rep_identity_.get());
        zmsg_add(msg, frIdentity);
        zmsg_addstr(msg, "");
        zmsg_addmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

        zmsg_send(&msg, riaps_socket_);


    }

    void DiscoveryMessageHandler::DhtPut(const std::string &key, std::vector<uint8_t> &data) {
        auto key_hash = dht::InfoHash::get(key);
        DhtPut(key_hash, data);
    }

    void DiscoveryMessageHandler::DhtPut(dht::InfoHash& keyhash,
                                         std::vector<uint8_t>& data) {
        DhtData d;
        if (has_security_) {
            d.EncryptData(data, private_key_);
        } else {
            d.raw_data = data;
        }

        dht_node_.put(keyhash, d);
    }

    void DiscoveryMessageHandler::HandleServiceLookup(riaps::discovery::ServiceLookupReq::Reader &msgServiceLookup) {
        auto client = msgServiceLookup.getClient();
        auto path   = msgServiceLookup.getPath();

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
        auto current_client = std::unique_ptr<ClientDetails>(new ClientDetails());
        current_client->app_name       = path.getAppName();
        current_client->actor_host     = riaps::framework::Network::GetIPAddress(); //client.getActorHost();
        current_client->port_name      = client.getPortName();
        current_client->actor_name     = client.getActorName();
        current_client->instance_name  = client.getInstanceName();
        current_client->is_local       = path.getScope() == riaps::discovery::Scope::LOCAL ? true : false;

        // Copy for the get callback
        ClientDetails currentClientTmp(*current_client);

        // Now using just the discovery service to register the interested clients
        if (client_subscriptions_.find(lookupkey.first) == client_subscriptions_.end()) {
            // Nobody subscribed to this messagetype
            client_subscriptions_[lookupkey.first] = std::vector<std::unique_ptr<ClientDetails>>();
        }

        if (std::find(client_subscriptions_[lookupkey.first].begin(),
                      client_subscriptions_[lookupkey.first].end(),
                      current_client) == client_subscriptions_[lookupkey.first].end()) {

            client_subscriptions_[lookupkey.first].push_back(std::move(current_client));
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

        auto serializedMessage = capnp::messageToFlatArray(message);

        zmsg_t *msg = zmsg_new();
        zframe_t* frIdentity = zframe_dup(rep_identity_.get());
        zmsg_add(msg, frIdentity);
        zmsg_addstr(msg, "");
        zmsg_addmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

        zmsg_send(&msg, riaps_socket_);

        // Start listener on this key if it hasn't been started yet.
        if (registered_listeners_.find(lookupkeyhash.toString()) == registered_listeners_.end()) {

            // Add listener to the added key
            auto logger = logger_;
            auto pk = private_key_;
            auto has_security = has_security_;
            registered_listeners_[lookupkeyhash.toString()] =
                    dht_node_.listen<DhtData>(lookupkeyhash,
                                     [lookupkey, logger, has_security, pk](vector<DhtData>&& values) {
                                         std::async(launch::async, [pk, has_security, logger, lookupkey](vector<DhtData> values){
                                             vector<string> update_results;
                                             for (DhtData& value :values) {
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
                                                 for (std::string provider : update_results) {
                                                     ::capnp::Text::Builder b(
                                                             (char *) provider.c_str());
                                                     msg_providers.set(
                                                             provider_index++,
                                                             b.asString());
                                                 }

                                                 auto serializedMessage = capnp::messageToFlatArray(
                                                         message);

                                                 zmsg_t *msg = zmsg_new();
                                                 zmsg_pushmem(msg,
                                                              serializedMessage.asBytes().begin(),
                                                              serializedMessage.asBytes().size());

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

        DhtGet(lookupkey.first, currentClientTmp,0);
    }

    void DiscoveryMessageHandler::DhtGet(const string lookupKey, ClientDetails clientDetails, uint8_t callLevel) {
        auto logger       = logger_;
        auto pk           = private_key_;
        auto has_security = has_security_;
        auto data_id = dht::InfoHash::get(lookupKey);
        dht_node_.get<DhtData>(data_id,
                      [clientDetails, logger, lookupKey, has_security, pk]
                              (DhtData&& value) {

                          async(launch::async, [pk, has_security, clientDetails](DhtData d){
                              if (has_security) {
                                  if (!d.DecryptData(pk)) return;
                              }
                              string result = string(d.raw_data.begin(), d.raw_data.end());

                              zsock_t *notify_ractor_socket = zsock_new_push(DHT_RESULT_CHANNEL);

                              capnp::MallocMessageBuilder message;
                              auto msg_providerlistpush = message.initRoot<riaps::discovery::DhtUpdate>();
                              riaps::discovery::ProviderListGet::Builder msg_providerget = msg_providerlistpush.initProviderGet();

                              auto msg_path = msg_providerget.initPath();

                              riaps::discovery::Scope scope = clientDetails.is_local ? riaps::discovery::Scope::LOCAL : riaps::discovery::Scope::GLOBAL;
                              string app_name = clientDetails.app_name;

                              msg_path.setScope(scope);
                              msg_path.setAppName(clientDetails.app_name);

                              auto msg_client = msg_providerget.initClient();

                              msg_client.setActorName(clientDetails.actor_name);
                              msg_client.setPortName(clientDetails.port_name);
                              msg_client.setActorHost(clientDetails.actor_host);
                              msg_client.setInstanceName(clientDetails.instance_name);

                              auto msg_get_results = msg_providerget.initResults(1);
                              msg_get_results.set(0, result);

                              auto serializedMessage = capnp::messageToFlatArray(message);

                              zmsg_t *msg = zmsg_new();
                              auto bytes = serializedMessage.asBytes();
                              zmsg_pushmem(msg, bytes.begin(), bytes.size());
                              zmsg_send(&msg, notify_ractor_socket);

                              sleep(1);
                              zsock_destroy(&notify_ractor_socket);
                              sleep(1);

                          }, value);

                          return true;
                      }, [logger, lookupKey, clientDetails, callLevel, this](bool success){
                    logger->debug("OpenDHT.Get({}) finished with '{}'  callback done!", lookupKey,  success );
                    if (!success) {
                        if (callLevel<10) {
                            std::thread t([lookupKey, clientDetails, callLevel, this]() {
                                zclock_sleep(1000);
                                this->DhtGet(lookupKey, clientDetails, callLevel + 1);
                            });
                            t.detach();
                        }
                    }

                }
        );
    }

    void DiscoveryMessageHandler::HandleGroupJoin(riaps::discovery::GroupJoinReq::Reader& msgGroupJoin){

        // Join to the group.
        auto msgGroupServices   = msgGroupJoin.getServices();
        string appName     = msgGroupJoin.getAppName();
        string componentId = msgGroupJoin.getComponentId();
        auto actorPid           = msgGroupJoin.getPid();

        riaps::groups::GroupDetails groupDetails;
        groupDetails.app_name     = appName;
        groupDetails.component_id = componentId;
        groupDetails.group_id = {
                msgGroupJoin.getGroupId().getGroupType(),
                msgGroupJoin.getGroupId().getGroupName()
        };

        for (int i = 0; i<msgGroupServices.size(); i++){
            groupDetails.group_services.push_back({
                                                         msgGroupServices[i].getMessageType(),
                                                         msgGroupServices[i].getAddress()
                                                 });
            logger_->debug("REG: {}", msgGroupServices[i].getAddress().cStr());
        }

        string key = fmt::format("/groups/{}",appName);

        //Send response
        capnp::MallocMessageBuilder repMessage;
        auto msgDiscoRep     = repMessage.initRoot<riaps::discovery::DiscoRep>();
        auto msgGroupJoinRep = msgDiscoRep.initGroupJoin();

        msgGroupJoinRep.setStatus(riaps::discovery::Status::OK);

        auto serializedMessage = capnp::messageToFlatArray(repMessage);

        zmsg_t *msg = zmsg_new();
        zframe_t* frIdentity = zframe_dup(rep_identity_.get());
        zmsg_add(msg, frIdentity);
        zmsg_addstr(msg, "");
        zmsg_addmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

        zmsg_send(&msg, riaps_socket_);

        dht_node_.get<DhtData>(dht::InfoHash(key), [this](vector<DhtData>&& values){
            if (values.size() == 0) return true;

            async(launch::async, &DiscoveryMessageHandler::PushDhtValuesToDisco, this, values);

            return true;
        });

        zclock_sleep(1000);

        /**
         * Store the details for renewing
         */

        if (group_services_.find(actorPid) == group_services_.end()){
            group_services_[actorPid] == std::vector<std::shared_ptr<RegisteredGroup>>();
        }
        auto currentGroupReg = std::make_shared<RegisteredGroup>(RegisteredGroup{
                key,
                groupDetails,
                actorPid,
                Timeout<std::chrono::minutes>(10) //10 minutes
        });
        group_services_[actorPid].push_back(std::move(currentGroupReg));

        auto group_details_blob = dht::packMsg<riaps::groups::GroupDetails>(groupDetails);
        DhtPut(key, group_details_blob);
    }

    void DiscoveryMessageHandler::PushDhtValuesToDisco(std::vector<DhtData>&& values) {
        zsock_t *dhtNotificationSocket = zsock_new_push(DHT_RESULT_CHANNEL);
        zsock_set_linger(dhtNotificationSocket, 0);
        zsock_set_sndtimeo(dhtNotificationSocket, 0);


        // Let's unpack the data
        for (auto& value : values) {
            if (has_security_) {
                if (!value.DecryptData(private_key_))
                    continue;
            }

            riaps::groups::GroupDetails v = dht::unpackMsg<riaps::groups::GroupDetails>(value.raw_data);

            capnp::MallocMessageBuilder dhtMessage;
            auto msgDhtUpdate = dhtMessage.initRoot<riaps::discovery::DhtUpdate>();
            auto msgGroupUpdate = msgDhtUpdate.initGroupUpdate();
            msgGroupUpdate.setComponentId(v.component_id);
            msgGroupUpdate.setAppName(v.app_name);

            auto groupId = msgGroupUpdate.initGroupId();
            groupId.setGroupName(v.group_id.group_name);
            groupId.setGroupType(v.group_id.group_type_id);

            auto groupServices = msgGroupUpdate.initServices(v.group_services.size());
            for (int i = 0; i<v.group_services.size(); i++){
                groupServices[i].setAddress(v.group_services[i].address);
                groupServices[i].setMessageType(v.group_services[i].message_type);
            }

            auto serializedMessage = capnp::messageToFlatArray(dhtMessage);

            zmsg_t *msg = zmsg_new();
            auto bytes = serializedMessage.asBytes();
            zmsg_pushmem(msg, bytes.begin(), bytes.size());
            zmsg_send(&msg, dhtNotificationSocket);
            //std::cout << "[DHT] Group notifications sent to discovery service" << std::endl;
        }

        zclock_sleep(100);
        zsock_destroy(&dhtNotificationSocket);
    }

    // Handle ZMQ messages, arriving on the zactor PIPE
    void DiscoveryMessageHandler::HandlePipeMessage(){
        zmsg_t* msg = zmsg_recv(pipe_);
        if (!msg){
            std::cout << "No msg => interrupted" << std::endl;
            terminated_ = true;
        }
        else {
            char *command = zmsg_popstr(msg);

            if (streq(command, "$TERM")) {
                logger_->info("$TERMINATE arrived, discovery service is stopping.");
                terminated_ = true;
            } else if (streq(command, CMD_DISCO_JOIN)) {
                bool hasMoreMsg = true;

                while (hasMoreMsg) {
                    char *newhost = zmsg_popstr(msg);
                    if (newhost) {
                        logger_->info("Join: {}", newhost);
                        string str_newhost(newhost);
                        dht_node_.bootstrap(str_newhost, std::to_string(RIAPS_DHT_NODE_PORT));
                        zstr_free(&newhost);
                    } else {
                        hasMoreMsg = false;
                    }
                }
            }

            if (command) {
                zstr_free(&command);
            }
            zmsg_destroy(&msg);
        }
    }

    bool DiscoveryMessageHandler::HandleZombieUpdate(std::vector<DhtData>&& values){
        std::vector<std::string> dhtResults;

        for (auto &value :values) {
            if (has_security_) {
                if (!value.DecryptData(private_key_))
                    continue;
            }
            string result = string(value.raw_data.begin(), value.raw_data.end());
            dhtResults.push_back(result);
        }

        if (dhtResults.size() > 0) {
            zsock_t *dhtNotification = zsock_new_push(DHT_RESULT_CHANNEL);
            capnp::MallocMessageBuilder message;

            auto msgDhtUpdate = message.initRoot<riaps::discovery::DhtUpdate>();
            auto msgZombieList = msgDhtUpdate.initZombieList(dhtResults.size());

            for (int i=0; i<dhtResults.size(); i++){
                char* cres = (char*)dhtResults[i].c_str();
                msgZombieList.set(i, capnp::Text::Builder(cres));
                logger_->info("Service is considered as zombie: {}", dhtResults[i].c_str());
            }

            auto serializedMessage = capnp::messageToFlatArray(message);

            zmsg_t *msg = zmsg_new();
            zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

            zmsg_send(&msg, dhtNotification);
            zclock_sleep(500);
            zsock_destroy(&dhtNotification);
        }
        return true;
    }

    void DiscoveryMessageHandler::HandleDhtGet(
                   const riaps::discovery::ProviderListGet::Reader& msgProviderGet,
                   const std::map<std::string, std::shared_ptr<ActorDetails>>& clients)
    {
        auto msgGetResults = msgProviderGet.getResults();

        for (int idx = 0; idx < msgGetResults.size(); idx++) {
            string resultEndpoint = string(msgGetResults[idx].cStr());

            if (zombie_services_.find(resultEndpoint)!=zombie_services_.end()) continue;

            auto pos = resultEndpoint.find(':');
            if (pos == string::npos) {
                continue;
            }

            string host = resultEndpoint.substr(0, pos);
            string port = resultEndpoint.substr(pos + 1, std::string::npos);
            int portNum = -1;

            try {
                portNum = stoi(port);
            } catch (invalid_argument &e) {
                logger_->error("Cast error, string -> int, portnumber: {}\n{}", port, e.what());
                continue;
            }
            catch (out_of_range &e) {
                logger_->error("Cast error, string -> int, portnumber: {}\n{}", port, e.what());
                continue;
            }

            capnp::MallocMessageBuilder message;
            auto msgDiscoUpd = message.initRoot<riaps::discovery::DiscoUpd>();
            auto msgPortUpd  = msgDiscoUpd.initPortUpdate();
            auto msgClient = msgPortUpd.initClient();
            auto msgSocket = msgPortUpd.initSocket();

            // Set up client
            msgClient.setActorHost(msgProviderGet.getClient().getActorHost());
            msgClient.setActorName(msgProviderGet.getClient().getActorName());
            msgClient.setInstanceName(msgProviderGet.getClient().getInstanceName());
            msgClient.setPortName(msgProviderGet.getClient().getPortName());

            msgPortUpd.setScope(msgProviderGet.getPath().getScope());

            msgSocket.setHost(host);
            msgSocket.setPort(portNum);

            auto serializedMessage = capnp::messageToFlatArray(message);

            zmsg_t *msg = zmsg_new();
            zmsg_pushmem(msg, serializedMessage.asBytes().begin(),
                         serializedMessage.asBytes().size());

            string clientKeyBase = fmt::format("/{}/{}/",
                                               msgProviderGet.getPath().getAppName().cStr(),
                                               msgProviderGet.getClient().getActorName().cStr());

            // Client might gone while the DHT was looking for the values
            if (clients.find(clientKeyBase) != clients.end()) {
                zmsg_send(&msg, clients.at(clientKeyBase)->socket);
                logger_->info("Get() returns {}@{}:{} to {}",
                               msgProviderGet.getClient().getPortName().cStr(),
                               host,
                               portNum,
                               clientKeyBase);

            } else {
                zmsg_destroy(&msg);
                logger_->warn("Get returned with values, but client has gone.");
            }
        }
    }

/// \brief Gets the provider updates from the OpenDHT (e.g.: new publisher registered) and sends a DiscoUpd message
///        to the interested clients.
/// \param msgProviderUpdate The capnp message from the OpenDHT Listen().
/// \param clientSubscriptions List of current key subscribtions.
/// \param clients  Holds the ZMQ sockets of the client actors.
    void DiscoveryMessageHandler::HandleDhtUpdate(const riaps::discovery::ProviderListUpdate::Reader& msgProviderUpdate,
                      const map<string, vector<unique_ptr<ClientDetails>>>& clientSubscriptions){

        string provider_key = string(msgProviderUpdate.getProviderpath().cStr());

        auto msg_newproviders = msgProviderUpdate.getNewvalues();

        // Look for services who may interested in the new provider
        if (clientSubscriptions.find(provider_key) != clientSubscriptions.end()) {
            for (auto &subscribedClient : clientSubscriptions.at(provider_key)) {
                for (int idx = 0; idx < msg_newproviders.size(); idx++) {
                    std::string new_provider_endpoint = std::string(msg_newproviders[idx].cStr());

                    // If the service marked as zombie
                    if (zombie_services_.find(new_provider_endpoint)!=zombie_services_.end()) continue;

                    auto pos = new_provider_endpoint.find(':');
                    if (pos == std::string::npos) {
                        continue;
                    }

                    string host = new_provider_endpoint.substr(0, pos);
                    string port = new_provider_endpoint.substr(pos + 1, std::string::npos);
                    int portNum = -1;

                    try {
                        portNum = std::stoi(port);
                    } catch (invalid_argument &e) {
                        cout << "Cast error, string -> int, portnumber: " << port << std::endl;
                        cout << e.what() << std::endl;
                        continue;
                    }
                    catch (out_of_range &e) {
                        cout << "Cast error, string -> int, portnumber: " << port << std::endl;
                        cout << e.what() << std::endl;
                        continue;
                    }

                    string clientKeyBase = fmt::format("/{}/{}/",
                                                       subscribedClient->app_name,
                                                       subscribedClient->actor_name);

                    logger_->info("Search for registered actor: {}", clientKeyBase);

                    // If the client port saved before
                    if (clients_.find(clientKeyBase) != clients_.end()) {
                        const ActorDetails *clientSocket = clients_.at(clientKeyBase).get();

                        if (clientSocket->socket != NULL) {
                            capnp::MallocMessageBuilder message;
                            auto msgDiscoUpd = message.initRoot<riaps::discovery::DiscoUpd>();
                            auto msgPortUpd  = msgDiscoUpd.initPortUpdate();
                            auto msgClient = msgPortUpd.initClient();
                            auto msgSocket = msgPortUpd.initSocket();

                            // Set up client
                            msgClient.setActorHost(subscribedClient->actor_host);
                            msgClient.setActorName(subscribedClient->actor_name);
                            msgClient.setInstanceName(subscribedClient->instance_name);
                            msgClient.setPortName(subscribedClient->port_name);

                            msgPortUpd.setScope(subscribedClient->is_local ? riaps::discovery::Scope::LOCAL : riaps::discovery::Scope::GLOBAL);

                            msgSocket.setHost(host);
                            msgSocket.setPort(portNum);

                            auto serializedMessage = capnp::messageToFlatArray(message);

                            zmsg_t *msg = zmsg_new();
                            zmsg_pushmem(msg, serializedMessage.asBytes().begin(),
                                         serializedMessage.asBytes().size());

                            zmsg_send(&msg, clientSocket->socket);

                            logger_->info("Update() returns {}@{}:{} to {}", subscribedClient->port_name, host, portNum, clientKeyBase);
                        }
                    }
                }
            }
        }
    }

    void DiscoveryMessageHandler::HandleDhtGroupUpdate(const riaps::discovery::GroupUpdate::Reader &msgGroupUpdate) {
        // Look for the affected actors
        string appName = msgGroupUpdate.getAppName().cStr();
        bool actorFound = false;

        set<zsock_t*> sentCache;

        for (auto& client : clients_){
            if (client.second->app_name == appName){
                if (sentCache.find(client.second->socket) != sentCache.end()) continue;

                std::string log;
                for (int i=0; i<msgGroupUpdate.getServices().size(); i++) {

                    log+=msgGroupUpdate.getServices()[i].getAddress().cStr();
                    log+="; ";
                }
                logger_->debug("UPD: {}", log);

                // Store the socket pointer to avoid multiple sending of the same update.
                sentCache.insert(client.second->socket);
                actorFound == true;

                capnp::MallocMessageBuilder builder;
                auto msgDiscoUpdate = builder.initRoot<riaps::discovery::DiscoUpd>();
                msgDiscoUpdate.setGroupUpdate(msgGroupUpdate);

                auto serializedMessage = capnp::messageToFlatArray(builder);

                zmsg_t *msg = zmsg_new();
                zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

                zmsg_send(&msg, client.second.get()->socket);
            }
        }

        // TODO: No active actor for this app, purge this listener
        if (!actorFound) {

        }
    }

    void DiscoveryMessageHandler::RenewServices() {
        int64_t now = zclock_mono();
        for (auto pidIt= service_checkins_.begin(); pidIt!=service_checkins_.end(); pidIt++){
            for(auto serviceIt = pidIt->second.begin(); serviceIt!=pidIt->second.end(); serviceIt++){

                // Renew
                if (now - (*serviceIt)->createdTime > (*serviceIt)->timeout){
                    (*serviceIt)->createdTime = now;

                    // Reput key-value
                    vector<uint8_t> opendht_data((*serviceIt)->value.begin(), (*serviceIt)->value.end());
                    auto keyhash = dht::InfoHash::get((*serviceIt)->key);
                    this->DhtPut(keyhash, opendht_data);
                }
            }
        }
    }

    void DiscoveryMessageHandler::MaintainZombieList(){
        int64_t currentTime = zclock_mono();

        auto it = zombie_services_.begin();
        while (it != zombie_services_.end()) {
            // Purge zombies after 15 minutes
            if ((currentTime - it->second) > 60*15*1000) {
                logger_->info("Purge zombie from cache: {}", it->first);
                it = zombie_services_.erase(it);
            } else {
                ++it;
            }
        }
    }

    int DiscoveryMessageHandler::DeregisterActor(const std::string& appName,
                                                 const std::string& actorName){

        string clientKeyBase   = fmt::format("/{}/{}/",appName,actorName);
        string clientKeyLocal  = clientKeyBase + mac_address_;
        string clientKeyGlobal = clientKeyBase + host_address_;

        vector<string> keysToBeErased {clientKeyBase, clientKeyLocal, clientKeyGlobal};

        logger_->info("Unregister actor: {}", clientKeyBase);

        int port = -1;
        if (clients_.find(clientKeyBase)!=clients_.end()){
            port = clients_[clientKeyBase]->port;
        }

        for (auto it = keysToBeErased.begin(); it!=keysToBeErased.end(); it++){
            if (clients_.find(*it)!=clients_.end()){

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

        for (auto it_client = clients_.begin(); it_client!=clients_.end(); it_client++){
            if (it_client->second->socket!=NULL) {
                zsock_destroy(&it_client->second->socket);
                it_client->second->socket=NULL;
            }
        }
        sleep(1);
    }

    const std::tuple<const string, const string> DiscoveryMessageHandler::BuildInsertKeyValuePair(
            const string&             appName,
            const string&             msgType,
            const riaps::discovery::Kind&  kind,
            const riaps::discovery::Scope& scope,
            const string&             host,
            const uint16_t                 port) {

        string key = fmt::format("/{}/{}/{}", appName, msgType, kindMap.at(kind));

        if (scope == riaps::discovery::Scope::LOCAL) {
            string mac_address = riaps::framework::Network::GetMacAddressStripped();
            key += mac_address;
        }

        const string value = fmt::format("{}:{}", host, to_string(port));

        return make_tuple(key, value);
    }

    const pair<const string, const string> DiscoveryMessageHandler::BuildLookupKey(
            const std::string& appName,
            const std::string& msgType,
            const riaps::discovery::Kind& kind,
            const riaps::discovery::Scope& scope,
            const std::string& clientActorHost,
            const std::string& clientActorName,
            const std::string& clientInstanceName,
            const std::string& clientPortName) {

        const map<riaps::discovery::Kind, std::string> kindPairs = {
                {riaps::discovery::Kind::CLT, kindMap.at(riaps::discovery::Kind::SRV)},
                {riaps::discovery::Kind::SUB, kindMap.at(riaps::discovery::Kind::PUB)},
                {riaps::discovery::Kind::REQ, kindMap.at(riaps::discovery::Kind::REP)},
                {riaps::discovery::Kind::REP, kindMap.at(riaps::discovery::Kind::REQ)},
                {riaps::discovery::Kind::QRY, kindMap.at(riaps::discovery::Kind::ANS)},
                {riaps::discovery::Kind::ANS, kindMap.at(riaps::discovery::Kind::QRY)}
        };

        string key = fmt::format("/{}/{}/{}", appName, msgType, kindPairs.at(kind));
        const string hostid = riaps::framework::Network::GetMacAddressStripped();

        if (scope == riaps::discovery::Scope::LOCAL) {
            key += hostid;
        }

        string client = fmt::format("/{}/{}/{}/{}/{}", appName, clientActorName, clientActorHost, clientInstanceName, clientPortName);

        if (scope == riaps::discovery::Scope::LOCAL) {
            client = client + ":" + hostid;
        }

        return {key, client};
    }
}
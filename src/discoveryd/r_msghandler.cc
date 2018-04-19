//
// Created by istvan on 10/6/17.
//

#include <discoveryd/r_msghandler.h>
#include <framework/rfw_configuration.h>
#include <framework/rfw_network_interfaces.h>
#include "../../include/discoveryd/r_msghandler.h"

namespace riaps{
    DiscoveryMessageHandler::DiscoveryMessageHandler(dht::DhtRunner &dhtNode, zsock_t** pipe, std::shared_ptr<spdlog::logger> logger)
        : m_dhtNode(dhtNode),
          m_pipe(*pipe),
          m_serviceCheckPeriod((uint16_t)20000), // 20 sec in in msec.
          m_zombieCheckPeriod((uint16_t)600000), // 10 min in msec
          m_zombieKey("/zombies"),
          m_terminated(false),
          m_logger(logger),
          m_repIdentity(nullptr),
          m_macAddress(riaps::framework::Network::GetMacAddressStripped()),
          m_hostAddress(riaps::framework::Network::GetIPAddress()){

    }

    bool DiscoveryMessageHandler::init() {
        m_dhtUpdateSocket = zsock_new_pull(DHT_ROUTER_CHANNEL);
        zsock_set_rcvtimeo(m_dhtUpdateSocket, 0);

        //m_riapsSocket = zsock_new(ZMQ_REP);
        m_riapsSocket = zsock_new(ZMQ_ROUTER);

        zsock_set_linger(m_riapsSocket, 0);
        zsock_set_sndtimeo(m_riapsSocket, 0);
        zsock_set_rcvtimeo(m_riapsSocket, 0);

        zsock_bind(m_riapsSocket, "%s", riaps::framework::Configuration::GetDiscoveryEndpoint().c_str());
        m_poller = zpoller_new(m_pipe, m_riapsSocket, m_dhtUpdateSocket, nullptr);
        zsock_signal (m_pipe, 0);
        m_lastServiceCheckin = m_lastZombieCheck = zclock_mono();
        // Get current zombies, and listen to new zombies
        m_dhtNode.get(m_zombieKey, [this](const std::vector<std::shared_ptr<dht::Value>> &values){
            return this->handleZombieUpdate(values);
        });
        // Subscribe for further zombies
        m_dhtNode.listen(m_zombieKey, [this](const std::vector<std::shared_ptr<dht::Value>> &values){
            return this->handleZombieUpdate(values);
        });
    }



    void DiscoveryMessageHandler::run() {
        while (!m_terminated){
            void *which = zpoller_wait(m_poller, REGULAR_MAINTAIN_PERIOD);

            // Check services whether they are still alive.
            // Reregister the too old services (OpenDHT ValueType settings, it is 10 minutes by default)
            int64_t loopStartTime = zclock_mono();
            if ((loopStartTime-m_lastServiceCheckin) > m_serviceCheckPeriod){

                // Obsolote, riaps-deplo should be asked about the reneewal
                //maintainRenewal();
            }

            // Check outdated zombies
            if ((loopStartTime-m_lastZombieCheck) > m_zombieCheckPeriod) {
                maintainZombieList();
            }

            // Handling messages from the caller (e.g.: $TERM$)
            if (which == m_pipe) {
                handlePipeMessage();
            }
            else if (which == m_dhtUpdateSocket){
                // Process the updated nodes
                zmsg_t* msgResponse = zmsg_recv(m_dhtUpdateSocket);

                zframe_t* capnpMsgBody = zmsg_pop(msgResponse);
                size_t    size = zframe_size(capnpMsgBody);
                byte*     data = zframe_data(capnpMsgBody);


                try {
                    auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word *>(data), size / sizeof(capnp::word));

                    capnp::FlatArrayMessageReader reader(capnp_data);
                    auto msgDhtUpdate = reader.getRoot<riaps::discovery::DhtUpdate>();

                    // If update
                    if (msgDhtUpdate.isProviderUpdate()) {
                        riaps::discovery::ProviderListUpdate::Reader msgProviderUpdate = msgDhtUpdate.getProviderUpdate();
                        handleDhtUpdate(msgProviderUpdate, m_clientSubscriptions);

                    } else if (msgDhtUpdate.isProviderGet()) {
                        riaps::discovery::ProviderListGet::Reader msgProviderGet = msgDhtUpdate.getProviderGet();
                        handleDhtGet(msgProviderGet, m_clients);
                    } else if (msgDhtUpdate.isZombieList()) {
                        auto zombieList = msgDhtUpdate.getZombieList();
                        for (int i =0; i< zombieList.size(); i++){
                            std::string currentZombie = zombieList[i];
                            m_zombieServices[currentZombie] = zclock_mono();
                        }
                    } else if (msgDhtUpdate.isGroupUpdate()){
                        riaps::discovery::GroupUpdate::Reader msgGroupUpdate = msgDhtUpdate.getGroupUpdate();
                        handleDhtGroupUpdate(msgGroupUpdate);
                    }

                    zframe_destroy(&capnpMsgBody);
                    zmsg_destroy(&msgResponse);
                }
                catch(kj::Exception& e){
                    m_logger->error("Couldn't deserialize message from DHT_ROUTER_SOCKET");

                    continue;

                }
            }

                // Handling messages from the RIAPS FW
            else if(which == m_riapsSocket){
                handleRiapsMessage();
            }
            else {
                //auto outdateds = maintain_servicecache(service_checkins);
            }
        }
    }

    void DiscoveryMessageHandler::handleRiapsMessage() {
        zmsg_t *riapsMessage = zmsg_recv(m_riapsSocket);
        if (!riapsMessage) {
            m_logger->critical("Empty message arrived => interrupted");
            m_terminated = true;
        } else {
            zframe_t* fr = zmsg_pop(riapsMessage);
            m_repIdentity.reset();
            m_repIdentity = std::shared_ptr<zframe_t>(fr, [this](zframe_t* oldFrame){
                if (oldFrame!=nullptr) {
                    zframe_destroy(&oldFrame);
                }
            });
            zframe_t* emptyFrame = zmsg_pop(riapsMessage);
            zframe_destroy(&emptyFrame);

            zframe_t *capnp_msgbody = zmsg_pop(riapsMessage);
            size_t size = zframe_size(capnp_msgbody);
            byte *data = zframe_data(capnp_msgbody);

            riaps::discovery::DiscoReq::Reader msgDiscoReq;

            // Convert ZMQ bytes to CAPNP buffer
            auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word *>(data), size / sizeof(capnp::word));

            capnp::FlatArrayMessageReader reader(capnp_data);
            msgDiscoReq = reader.getRoot<riaps::discovery::DiscoReq>();

            if (msgDiscoReq.isActorReg()) {
                riaps::discovery::ActorRegReq::Reader msgActorReq = msgDiscoReq.getActorReg();
                handleActorReg(msgActorReq);
            } else if (msgDiscoReq.isActorUnreg()){
                riaps::discovery::ActorUnregReq::Reader msgActorUnreg = msgDiscoReq.getActorUnreg();
                handleActorUnreg(msgActorUnreg);
            } else if (msgDiscoReq.isServiceReg()){
                riaps::discovery::ServiceRegReq::Reader msgServiceReg = msgDiscoReq.getServiceReg();
                handleServiceReg(msgServiceReg);
            } else if (msgDiscoReq.isServiceLookup()){
                riaps::discovery::ServiceLookupReq::Reader msgServiceLookup = msgDiscoReq.getServiceLookup();
                handleServiceLookup(msgServiceLookup);
            } else if (msgDiscoReq.isGroupJoin()){
                riaps::discovery::GroupJoinReq::Reader msgGroupJoin = msgDiscoReq.getGroupJoin();
                handleGroupJoin(msgGroupJoin);
            }
            zmsg_destroy(&riapsMessage);
            zframe_destroy(&capnp_msgbody);
        }


    }

    void DiscoveryMessageHandler::handleActorReg(riaps::discovery::ActorRegReq::Reader& msgActorReq) {

        std::string actorname = std::string(msgActorReq.getActorName().cStr());
        std::string appName   = std::string(msgActorReq.getAppName().cStr());

        std::string clientKeyBase = fmt::format("/{}/{}/",appName, actorname); //"/" + appName + '/' + actorname + "/";
        m_logger->info("Register actor with PID - {} : {}", msgActorReq.getPid(), clientKeyBase);

        auto registeredActorIt = m_clients.find(clientKeyBase);
        bool isRegistered =  registeredActorIt!= m_clients.end();

        // If the name of the actor is registered previously and it still run => Error
        bool isRunning = isRegistered && kill(registeredActorIt->second->pid, 0) == 0;

        // If the actor already registered and running
        if (isRunning) {
            m_logger->error("Cannot register actor. This actor already registered ({})", clientKeyBase);

            capnp::MallocMessageBuilder message;
            auto drepmsg = message.initRoot<riaps::discovery::DiscoRep>();
            auto arepmsg = drepmsg.initActorReg();
            arepmsg.setPort(0);
            arepmsg.setStatus(riaps::discovery::Status::ERR);


            auto serializedMessage = capnp::messageToFlatArray(message);

            zmsg_t *msg = zmsg_new();
            zframe_t* frIdentity = zframe_dup(m_repIdentity.get());
            zmsg_add(msg, frIdentity);
            zmsg_addstr(msg, "");
            zmsg_addmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());
            zmsg_send(&msg, m_riapsSocket);

        } else {

            // Purge the old instance
            if (isRegistered && !isRunning) {
                deregisterActor(appName, actorname);
            }

            // Open a new PAIR socket for actor communication
            zsock_t *actor_socket = zsock_new (ZMQ_PAIR);
            auto port = zsock_bind(actor_socket, "tcp://*:!");

            //_clients[clientKeyBase] = std::unique_ptr<actor_details_t>(new actor_details_t());
            m_clients[clientKeyBase] =  std::make_shared<ActorDetails>();
            m_clients[clientKeyBase] -> socket  = actor_socket;
            m_clients[clientKeyBase] -> port    = port;
            m_clients[clientKeyBase] -> pid     = msgActorReq.getPid();
            m_clients[clientKeyBase] -> appName = appName;

            // Subscribe to groups
            if (m_groupListeners.find(appName) == m_groupListeners.end()) {
                std::string key = "/groups/"+appName;
                m_groupListeners[appName] =
                        m_dhtNode.listen(key, [this](const std::vector<std::shared_ptr<dht::Value>> &values){
                            if (values.size() == 0) return true;

                            std::thread t(&DiscoveryMessageHandler::pushDhtValuesToDisco, this, values);
                            t.detach();



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
            zframe_t* frIdentity = zframe_dup(m_repIdentity.get());
            zmsg_add(msg, frIdentity);
            zmsg_addstr(msg, "");
            zmsg_addmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

            zmsg_send(&msg, m_riapsSocket);

            // Use the same object, do not create another copy of actor_details;
            std::string clientKeyLocal = clientKeyBase + m_macAddress;
            m_clients[clientKeyLocal] = m_clients[clientKeyBase];
            //_clients[clientKeyLocal] = std::unique_ptr<actor_details_t>(new actor_details_t());
            //_clients[clientKeyLocal]->port = port;
            //_clients[clientKeyLocal]->pid = msgActorReq.getPid();

            std::string clientKeyGlobal = clientKeyBase + m_hostAddress;
            m_clients[clientKeyGlobal] = m_clients[clientKeyBase];
            //_clients[clientKeyGlobal] = std::unique_ptr<actor_details_t>(new actor_details_t());
            //_clients[clientKeyGlobal]->port = port;
            //_clients[clientKeyGlobal]->pid = msgActorReq.getPid();
        }
    }

    void DiscoveryMessageHandler::handleActorUnreg(riaps::discovery::ActorUnregReq::Reader &msgActorUnreg) {
        std::string actorname = std::string(msgActorUnreg.getActorName().cStr());
        std::string appname = std::string(msgActorUnreg.getAppName().cStr());
        int servicePid = msgActorUnreg.getPid();

        // Mark actor's services as zombie
        if (m_serviceCheckins.find(servicePid)!=m_serviceCheckins.end()){
            for (auto& service : m_serviceCheckins[servicePid]){
                std::string serviceAddress = service->value;
                std::vector<uint8_t> opendht_data(serviceAddress.begin(), serviceAddress.end());
                m_dhtNode.put(m_zombieKey, dht::Value(opendht_data));
            }
        }

        int port = deregisterActor(appname, actorname);

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
        zframe_t* frIdentity = zframe_dup(m_repIdentity.get());
        zmsg_add(msg, frIdentity);
        zmsg_addstr(msg, "");
        zmsg_addmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

        zmsg_send(&msg, m_riapsSocket);
    }

    void DiscoveryMessageHandler::handleServiceReg(riaps::discovery::ServiceRegReq::Reader &msgServiceReg) {
        auto msgPath           = msgServiceReg.getPath();
        auto msgSock           = msgServiceReg.getSocket();
        auto servicePid         = msgServiceReg.getPid();


        auto kv_pair = buildInsertKeyValuePair(msgPath.getAppName(),
                                               msgPath.getMsgType(),
                                               msgPath.getKind(),
                                               msgPath.getScope(),
                                               msgSock.getHost(),
                                               msgSock.getPort());

        //std::cout << "Register service: " + std::get<0>(kv_pair) << std::endl;
        m_logger->info("Register service: {}", std::get<0>(kv_pair));

        // New pid
        if (m_serviceCheckins.find(servicePid) == m_serviceCheckins.end()) {
            m_serviceCheckins[servicePid] = std::vector<std::unique_ptr<ServiceCheckins>>();
        }

        // Add PID - Service Details
        std::unique_ptr<ServiceCheckins> newItem = std::unique_ptr<ServiceCheckins>(new ServiceCheckins());
        newItem->createdTime = zclock_mono();
        newItem->key         = std::get<0>(kv_pair);
        newItem->value       = std::get<1>(kv_pair);
        newItem->pid         = servicePid;
        m_serviceCheckins[servicePid].push_back(std::move(newItem));

        // Convert the value to bytes
        std::vector<uint8_t> opendht_data(std::get<1>(kv_pair).begin(), std::get<1>(kv_pair).end());
        auto keyhash = dht::InfoHash::get(std::get<0>(kv_pair));

        m_dhtNode.put(keyhash, dht::Value(opendht_data));

        //Send response
        capnp::MallocMessageBuilder message;
        auto msg_discorep = message.initRoot<riaps::discovery::DiscoRep>();
        auto msg_servicereg_rep = msg_discorep.initServiceReg();

        msg_servicereg_rep.setStatus(riaps::discovery::Status::OK);

        auto serializedMessage = capnp::messageToFlatArray(message);

        zmsg_t *msg = zmsg_new();
        zframe_t* frIdentity = zframe_dup(m_repIdentity.get());
        zmsg_add(msg, frIdentity);
        zmsg_addstr(msg, "");
        zmsg_addmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

        zmsg_send(&msg, m_riapsSocket);
    }

    void DiscoveryMessageHandler::handleServiceLookup(riaps::discovery::ServiceLookupReq::Reader &msgServiceLookup) {
        auto client = msgServiceLookup.getClient();
        auto path   = msgServiceLookup.getPath();

        // Key   -> /appname/msgType/kind
        // Value -> /appname/clientactorname/clienthost/clientinstancename/clientportname
        // The "value" is interested in "key"
        auto lookupkey =
                buildLookupKey(path.getAppName(),
                               path.getMsgType(),
                               path.getKind(),
                               path.getScope(),
                               client.getActorHost(),
                               client.getActorName(),
                               client.getInstanceName(),
                               client.getPortName());

        // This client is interested in this kind of messages. Register it.
        auto current_client = std::unique_ptr<ClientDetails>(new ClientDetails());
        current_client->app_name      = path.getAppName();
        current_client->actor_host    = client.getActorHost();
        current_client->portname      = client.getPortName();
        current_client->actor_name    = client.getActorName();
        current_client->instance_name = client.getInstanceName();
        current_client->isLocal       = path.getScope() == riaps::discovery::Scope::LOCAL ? true : false;

        // Copy for the get callback
        ClientDetails currentClientTmp(*current_client);

        // Now using just the discovery service to register the interested clients
        if (m_clientSubscriptions.find(lookupkey.first) == m_clientSubscriptions.end()) {
            // Nobody subscribed to this messagetype
            m_clientSubscriptions[lookupkey.first] = std::vector<std::unique_ptr<ClientDetails>>();
        }

        if (std::find(m_clientSubscriptions[lookupkey.first].begin(),
                      m_clientSubscriptions[lookupkey.first].end(),
                      current_client) == m_clientSubscriptions[lookupkey.first].end()) {

            m_clientSubscriptions[lookupkey.first].push_back(std::move(current_client));
        }

        dht::InfoHash lookupkeyhash = dht::InfoHash::get(lookupkey.first);

        //std::cout << "Get: " + lookupkey.first << std::endl;
        m_logger->info("Get: {}", lookupkey.first);

        //auto zombieServicesCopy = _zombieServices;
        m_dhtNode.get(lookupkey.first,
                    [currentClientTmp, lookupkey]
                            (const std::vector<std::shared_ptr<dht::Value>> &values) {
                        // Done Callback

                        std::vector<std::string> dhtGetResults;
                        for (const auto &value :values) {
                            std::string result = std::string(value->data.begin(), value->data.end());
                            dhtGetResults.push_back(result);
                        }

                        // Remove elements if they considered as zombie
//                        dhtGetResults.erase(
//                                std::remove_if(dhtGetResults.begin(),
//                                               dhtGetResults.end(),
//                                               [&zombieServicesCopy](const std::string& s) {
//                                                   auto z = zombieServicesCopy;
//                                                   auto b = zombieServicesCopy.find(s)!=zombieServicesCopy.end();
//                                                   return b;
//                                               }), dhtGetResults.end());

                        // IF there are still some results.
                        if (dhtGetResults.size() > 0) {

                            zsock_t *notify_ractor_socket = zsock_new_push(DHT_ROUTER_CHANNEL);

                            capnp::MallocMessageBuilder message;
                            auto msg_providerlistpush = message.initRoot<riaps::discovery::DhtUpdate>();
                            auto msg_providerget = msg_providerlistpush.initProviderGet();

                            auto msg_path = msg_providerget.initPath();

                            riaps::discovery::Scope scope = currentClientTmp.isLocal ? riaps::discovery::Scope::LOCAL : riaps::discovery::Scope::GLOBAL;
                            std::string app_name = currentClientTmp.app_name;

                            msg_path.setScope(scope);
                            msg_path.setAppName(currentClientTmp.app_name);

                            auto msg_client = msg_providerget.initClient();

                            msg_client.setActorName(currentClientTmp.actor_name);
                            msg_client.setPortName(currentClientTmp.portname);
                            msg_client.setActorHost(currentClientTmp.actor_host);
                            msg_client.setInstanceName(currentClientTmp.instance_name);

                            auto number_of_results = dhtGetResults.size();
                            ::capnp::List<::capnp::Text>::Builder get_results = msg_providerget.initResults(
                                    number_of_results);

                            int provider_index = 0;
                            for (std::string provider : dhtGetResults) {
                                char *c = (char *) provider.c_str();
                                ::capnp::Text::Builder B(c, provider.length());
                                get_results.set(provider_index++, B.asString());
                            }

                            auto serializedMessage = capnp::messageToFlatArray(message);

                            zmsg_t *msg = zmsg_new();
                            auto bytes = serializedMessage.asBytes();
                            zmsg_pushmem(msg, bytes.begin(), bytes.size());
                            zmsg_send(&msg, notify_ractor_socket);
                            //std::cout << "Get results sent to discovery service" << std::endl;


                            sleep(1);
                            zsock_destroy(&notify_ractor_socket);
                            sleep(1);
                        } else {
                            //std::cout << "Get has no results for query: " << lookupkey.first << std::endl;
                        }

                        return true;
                    }//, [=](bool success){
                // Done Callback
                // TODO: remove
                //sleep(1);
                //std::cout << "Get callback done! " <<std::endl;
                //done_cb(success);


                //}
        );






        //Send response
        capnp::MallocMessageBuilder message;
        auto msg_discorep = message.initRoot<riaps::discovery::DiscoRep>();
        auto msg_service_lookup_rep = msg_discorep.initServiceLookup();

        msg_service_lookup_rep.setStatus(riaps::discovery::Status::OK);

        auto number_of_clients = 0;
        auto sockets = msg_service_lookup_rep.initSockets(number_of_clients);

        auto serializedMessage = capnp::messageToFlatArray(message);

        zmsg_t *msg = zmsg_new();
        zframe_t* frIdentity = zframe_dup(m_repIdentity.get());
        zmsg_add(msg, frIdentity);
        zmsg_addstr(msg, "");
        zmsg_addmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

        zmsg_send(&msg, m_riapsSocket);

        // Start listener on this key if it hasn't been started yet.
        if (m_registeredListeners.find(lookupkeyhash.toString()) == m_registeredListeners.end()) {

            // Add listener to the added key
            m_registeredListeners[lookupkeyhash.toString()] = m_dhtNode.listen(lookupkeyhash,
                                                                           [lookupkey](
                                                                                   const std::vector<std::shared_ptr<dht::Value>> &values) {


                                                                               std::vector<std::string> update_results;
                                                                               for (const auto &value :values) {
                                                                                   std::string result = std::string(
                                                                                           value->data.begin(),
                                                                                           value->data.end());
                                                                                   update_results.push_back(result);
                                                                               }

                                                                               // Remove elements if they considered as zombie
//                                                                               update_results.erase(
//                                                                                       std::remove_if(update_results.begin(),
//                                                                                                      update_results.end(),
//                                                                                                      [&zombieServicesCopy](const std::string& s) {
//                                                                                                          auto z = zombieServicesCopy;
//                                                                                                          auto b = zombieServicesCopy.find(s)!=zombieServicesCopy.end();
//                                                                                                          return b;
//                                                                                                      }), update_results.end());

                                                                               if (update_results.size()>0) {

                                                                                   zsock_t *notify_ractor_socket = zsock_new_push(
                                                                                           DHT_ROUTER_CHANNEL);

                                                                                   capnp::MallocMessageBuilder message;


                                                                                   auto msg_providerlist_push = message.initRoot<riaps::discovery::DhtUpdate>();
                                                                                   auto msg_provider_update = msg_providerlist_push.initProviderUpdate();
                                                                                   msg_provider_update.setProviderpath(
                                                                                           lookupkey.first);


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

//                                                                                   std::cout
//                                                                                           << "Changes sent to discovery service: "
//                                                                                           << lookupkey.first
//                                                                                           << std::endl;

                                                                                   sleep(1);
                                                                                   zsock_destroy(
                                                                                           &notify_ractor_socket);
                                                                                   sleep(1);
                                                                               }

                                                                               return true; // keep listening
                                                                           }
            );
        }





    }

    void DiscoveryMessageHandler:: handleGroupJoin(riaps::discovery::GroupJoinReq::Reader& msgGroupJoin){

        // Join to the group.
        auto msgGroupServices   = msgGroupJoin.getServices();
        std::string appName     = msgGroupJoin.getAppName();
        std::string componentId = msgGroupJoin.getComponentId();
        auto actorPid           = msgGroupJoin.getPid();

        riaps::groups::GroupDetails groupDetails;
        groupDetails.appName     = appName;
        groupDetails.componentId = componentId;
        groupDetails.groupId = {
                msgGroupJoin.getGroupId().getGroupType(),
                msgGroupJoin.getGroupId().getGroupName()
        };

        for (int i = 0; i<msgGroupServices.size(); i++){
            groupDetails.groupServices.push_back({
                                                         msgGroupServices[i].getMessageType(),
                                                         msgGroupServices[i].getAddress()
                                                 });
            m_logger->debug("REG: {}", msgGroupServices[i].getAddress().cStr());
        }

//        msgpack::sbuffer sbuf;
//        msgpack::pack(sbuf, groupDetails);
        std::string key = "/groups/"+appName;
//        dht::Blob b(sbuf.data(), sbuf.data()+sbuf.size());
//        _dhtNode.put(key, dht::Value(b));




//        _dhtNode.put(key, dht::Value::pack(groupDetails),[this, key](bool succ){
//            std::thread t ([this, key, succ](){
//                if (!succ) zclock_sleep(5000);
//                this->_dhtNode.get(key, [this](const std::vector<std::shared_ptr<dht::Value>> &values){
//                    if (values.size() == 0) return true;
//
//                    std::thread t(&DiscoveryMessageHandler::PushDhtValuesToDisco, this, values);
//                    t.detach();
//
//                    return true;
//                });
//            });
//            t.detach();
//
//        });





        // Debug
//        std::cout << "Component joined to group: "
//                  << appName << "::"
//                  << groupDetails.groupId.groupTypeId << "::"
//                  << groupDetails.groupId.groupName   << std::endl;
//        std::cout << "Group services: " << std::endl;
//        for (auto& g : groupDetails.groupServices){
//            std::cout << "\t- " << g.address << " " << g.messageType << std::endl;
//        }

        //Send response
        capnp::MallocMessageBuilder repMessage;
        auto msgDiscoRep     = repMessage.initRoot<riaps::discovery::DiscoRep>();
        auto msgGroupJoinRep = msgDiscoRep.initGroupJoin();

        msgGroupJoinRep.setStatus(riaps::discovery::Status::OK);

        auto serializedMessage = capnp::messageToFlatArray(repMessage);

        zmsg_t *msg = zmsg_new();
        zframe_t* frIdentity = zframe_dup(m_repIdentity.get());
        zmsg_add(msg, frIdentity);
        zmsg_addstr(msg, "");
        zmsg_addmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

        zmsg_send(&msg, m_riapsSocket);

        //zclock_sleep(5000);

        m_dhtNode.get(key, [this](const std::vector<std::shared_ptr<dht::Value>> &values){
            if (values.size() == 0) return true;

            std::thread t(&DiscoveryMessageHandler::pushDhtValuesToDisco, this, values);
            t.detach();

            return true;
        });

        zclock_sleep(1000);

        /**
         * Store the details for renewing
         */

        if (m_groupServices.find(actorPid) == m_groupServices.end()){
            m_groupServices[actorPid] == std::vector<std::shared_ptr<RegisteredGroup>>();
        }
        auto currentGroupReg = std::make_shared<RegisteredGroup>(RegisteredGroup{
                key,
                groupDetails,
                actorPid,
                Timeout<std::ratio<60>>(std::chrono::duration<int, std::ratio<60>>(10)) //10 minutes
        });
        m_groupServices[actorPid].push_back(std::move(currentGroupReg));

        m_dhtNode.put(key, dht::Value::pack(groupDetails));
    }

    void DiscoveryMessageHandler::pushDhtValuesToDisco(std::vector<std::shared_ptr<dht::Value>> values) {
        zsock_t *dhtNotificationSocket = zsock_new_push(DHT_ROUTER_CHANNEL);
        zsock_set_linger(dhtNotificationSocket, 0);
        zsock_set_sndtimeo(dhtNotificationSocket, 0);


        // Let's unpack the data
        for (auto& value : values) {
            riaps::groups::GroupDetails v = value->unpack<riaps::groups::GroupDetails>();

            capnp::MallocMessageBuilder dhtMessage;
            auto msgDhtUpdate = dhtMessage.initRoot<riaps::discovery::DhtUpdate>();
            auto msgGroupUpdate = msgDhtUpdate.initGroupUpdate();
            msgGroupUpdate.setComponentId(v.componentId);
            msgGroupUpdate.setAppName(v.appName);

            auto groupId = msgGroupUpdate.initGroupId();
            groupId.setGroupName(v.groupId.groupName);
            groupId.setGroupType(v.groupId.groupTypeId);

            auto groupServices = msgGroupUpdate.initServices(v.groupServices.size());
            for (int i = 0; i<v.groupServices.size(); i++){
                groupServices[i].setAddress(v.groupServices[i].address);
                groupServices[i].setMessageType(v.groupServices[i].messageType);
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
    void DiscoveryMessageHandler::handlePipeMessage(){
        zmsg_t* msg = zmsg_recv(m_pipe);
        if (!msg){
            std::cout << "No msg => interrupted" << std::endl;
            m_terminated = true;
        }
        else {

            char *command = zmsg_popstr(msg);

            if (streq(command, "$TERM")) {
                //std::cout << std::endl << "$TERMINATE arrived, discovery service is stopping..." << std::endl;
                m_logger->info("$TERMINATE arrived, discovery service is stopping.");
                m_terminated = true;
            } else if (streq(command, CMD_JOIN)) {
                //std::cout << "New peer on the network. Join()" << std::endl;

                bool hasMoreMsg = true;

                while (hasMoreMsg) {
                    char *newhost = zmsg_popstr(msg);
                    if (newhost) {
                        //std::cout << "Join to: " << newhost << std::endl;
                        m_logger->info("Join to: {}", newhost);
                        std::string str_newhost(newhost);
                        //dhtJoinToCluster(str_newhost, RIAPS_DHT_NODE_PORT, dhtNode);
                        m_dhtNode.bootstrap(str_newhost, std::to_string(RIAPS_DHT_NODE_PORT));
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

    bool DiscoveryMessageHandler::handleZombieUpdate(const std::vector<std::shared_ptr<dht::Value>> &values){
        std::vector<std::string> dhtResults;

        for (const auto &value :values) {
            std::string result = std::string(value->data.begin(), value->data.end());
            dhtResults.push_back(result);
        }

        if (dhtResults.size() > 0) {
            zsock_t *dhtNotification = zsock_new_push(DHT_ROUTER_CHANNEL);
            capnp::MallocMessageBuilder message;

            auto msgDhtUpdate = message.initRoot<riaps::discovery::DhtUpdate>();
            auto msgZombieList = msgDhtUpdate.initZombieList(dhtResults.size());

            for (int i=0; i<dhtResults.size(); i++){
                char* cres = (char*)dhtResults[i].c_str();
                msgZombieList.set(i, capnp::Text::Builder(cres));
                //std::cout << "Service is considered as zombie: " << dhtResults[i].c_str() << std::endl;
                m_logger->info("Service is considered as zombie: {}", dhtResults[i].c_str());
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

    void DiscoveryMessageHandler::handleDhtGet(
                   const riaps::discovery::ProviderListGet::Reader& msgProviderGet,
                   const std::map<std::string, std::shared_ptr<ActorDetails>>& clients)
    {
        auto msgGetResults = msgProviderGet.getResults();

        for (int idx = 0; idx < msgGetResults.size(); idx++) {
            std::string resultEndpoint = std::string(msgGetResults[idx].cStr());

            if (m_zombieServices.find(resultEndpoint)!=m_zombieServices.end()) continue;

            auto pos = resultEndpoint.find(':');
            if (pos == std::string::npos) {
                continue;
            }

            std::string host = resultEndpoint.substr(0, pos);
            std::string port = resultEndpoint.substr(pos + 1, std::string::npos);
            int portNum = -1;

            try {
                portNum = std::stoi(port);
            } catch (std::invalid_argument &e) {
                std::cout << "Cast error, string -> int, portnumber: " << port << std::endl;
                std::cout << e.what() << std::endl;
                continue;
            }
            catch (std::out_of_range &e) {
                std::cout << "Cast error, string -> int, portnumber: " << port << std::endl;
                std::cout << e.what() << std::endl;
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

            std::string clientKeyBase =    "/" + std::string(msgProviderGet.getPath().getAppName().cStr())
                                         + "/" + std::string(msgProviderGet.getClient().getActorName().cStr())
                                         + "/";

            // Client might gone while the DHT was looking for the values
            if (clients.find(clientKeyBase) != clients.end()) {
                zmsg_send(&msg, clients.at(clientKeyBase)->socket);
                //std::cout << "Get results were sent to the client: " << clientKeyBase << std::endl;
                m_logger->info("Get results were sent to the client: {}", clientKeyBase);
            } else {
                zmsg_destroy(&msg);
                m_logger->warn("Get returned with values, but client has gone.");
                //std::cout << "Get returned with values, but client has gone. " << std::endl;
            }


        }
    }

/// \brief Gets the provider updates from the OpenDHT (e.g.: new publisher registered) and sends a DiscoUpd message
///        to the interested clients.
/// \param msgProviderUpdate The capnp message from the OpenDHT Listen().
/// \param clientSubscriptions List of current key subscribtions.
/// \param clients  Holds the ZMQ sockets of the client actors.
    void DiscoveryMessageHandler::handleDhtUpdate(const riaps::discovery::ProviderListUpdate::Reader& msgProviderUpdate,
                      const std::map<std::string, std::vector<std::unique_ptr<ClientDetails>>>& clientSubscriptions){

        std::string provider_key = std::string(msgProviderUpdate.getProviderpath().cStr());

        auto msg_newproviders = msgProviderUpdate.getNewvalues();

        // Look for services who may interested in the new provider
        if (clientSubscriptions.find(provider_key) != clientSubscriptions.end()) {
            for (auto &subscribedClient : clientSubscriptions.at(provider_key)) {
                for (int idx = 0; idx < msg_newproviders.size(); idx++) {
                    std::string new_provider_endpoint = std::string(msg_newproviders[idx].cStr());

                    // If the service marked as zombie
                    if (m_zombieServices.find(new_provider_endpoint)!=m_zombieServices.end()) continue;

                    auto pos = new_provider_endpoint.find(':');
                    if (pos == std::string::npos) {
                        continue;
                    }

                    std::string host = new_provider_endpoint.substr(0, pos);
                    std::string port = new_provider_endpoint.substr(pos + 1, std::string::npos);
                    int portNum = -1;

                    try {
                        portNum = std::stoi(port);
                    } catch (std::invalid_argument &e) {
                        std::cout << "Cast error, string -> int, portnumber: " << port << std::endl;
                        std::cout << e.what() << std::endl;
                        continue;
                    }
                    catch (std::out_of_range &e) {
                        std::cout << "Cast error, string -> int, portnumber: " << port << std::endl;
                        std::cout << e.what() << std::endl;
                        continue;
                    }

                    std::string clientKeyBase = fmt::format("/{}/{}/",
                                                            subscribedClient->app_name,
                                                            subscribedClient->actor_name);

                    m_logger->info("Search for registered actor: {}", clientKeyBase);

                    // Python reference:
                    // TODO: Figure out, de we really need for this. I don't think so...
                    //if self.hostAddress != actorHost:
                    //continue

                    // If the client port saved before
                    if (m_clients.find(clientKeyBase) != m_clients.end()) {
                        const ActorDetails *clientSocket = m_clients.at(clientKeyBase).get();

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
                            msgClient.setPortName(subscribedClient->portname);

                            msgPortUpd.setScope(subscribedClient->isLocal ? riaps::discovery::Scope::LOCAL : riaps::discovery::Scope::GLOBAL);

                            msgSocket.setHost(host);
                            msgSocket.setPort(portNum);

                            auto serializedMessage = capnp::messageToFlatArray(message);

                            zmsg_t *msg = zmsg_new();
                            zmsg_pushmem(msg, serializedMessage.asBytes().begin(),
                                         serializedMessage.asBytes().size());

                            zmsg_send(&msg, clientSocket->socket);

                            //std::cout << "Port update sent to the client: " << clientKeyBase << std::endl;
                            m_logger->info("Port update sent to the client: {}", clientKeyBase);
                        }
                    }
                }
            }
        }
    }

    void DiscoveryMessageHandler::handleDhtGroupUpdate(const riaps::discovery::GroupUpdate::Reader &msgGroupUpdate) {
        // Look for the affected actors
        std::string appName = msgGroupUpdate.getAppName().cStr();
        bool actorFound = false;

        std::set<zsock_t*> sentCache;

        for (auto& client : m_clients){
            if (client.second->appName == appName){
                if (sentCache.find(client.second->socket) != sentCache.end()) continue;

                std::string log;
                for (int i=0; i<msgGroupUpdate.getServices().size(); i++) {

                    log+=msgGroupUpdate.getServices()[i].getAddress().cStr();
                    log+="; ";
                }
                m_logger->debug("UPD: {}", log);

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

    // Should be depriacated, riaps-deplo is needed to figure out what runs
    void DiscoveryMessageHandler::maintainRenewalDepricated(){

        std::set<pid_t> purgeServices;
        std::set<pid_t> purgeGroups;

        // Collect terminated component services (pub/rep ports)
        for (auto it= m_serviceCheckins.begin(); it!=m_serviceCheckins.end(); it++){
            // Check pid, mark the removable pids
            if (!kill(it->first,0)==0){
                purgeServices.insert(it->first);
            }
        }

        // Collect groups with terminated parent component
        for (auto it= m_groupServices.begin(); it!=m_groupServices.end(); it++){
            // Check pid, mark the removable pids
            if (!kill(it->first,0)==0){
                m_logger->info("Remove group services with PID: {}", it->first);
                purgeGroups.insert(it->first);
            } else {
                for(auto& groupService : m_groupServices[it->first]) {
                    if (!groupService->timeout.IsTimeout()) continue;
                    m_dhtNode.put(groupService->groupKey, groupService->services);
                    groupService->timeout.Reset();
                }
            }
        }


        // Delete terminated groups from cache, keep the values in openDHT, just don't renew them
        for (auto pid : purgeGroups) {
            m_groupServices.erase(pid);
        }

        for (auto it = purgeServices.begin(); it!=purgeServices.end(); it++){
            //std::cout << "Remove services with PID: " << *it << std::endl;
            m_logger->info("Remove component services with PID: {}", *it);

            // Put the service address into the zombies list in DHT
            for (auto serviceIt  = m_serviceCheckins[*it].begin();
                 serviceIt != m_serviceCheckins[*it].end();
                 serviceIt++) {

                // host:port
                std::string serviceAddress = (*serviceIt)->value;
                std::vector<uint8_t> opendht_data(serviceAddress.begin(), serviceAddress.end());
                m_dhtNode.put(m_zombieKey, dht::Value(opendht_data));
            }

            m_serviceCheckins.erase(*it);
        }


        int64_t now = zclock_mono();
        for (auto pidIt= m_serviceCheckins.begin(); pidIt!=m_serviceCheckins.end(); pidIt++){
            for(auto serviceIt = pidIt->second.begin(); serviceIt!=pidIt->second.end(); serviceIt++){

                // Renew
                if (now - (*serviceIt)->createdTime > (*serviceIt)->timeout){
                    (*serviceIt)->createdTime = now;

                    // Reput key-value
                    std::vector<uint8_t> opendht_data((*serviceIt)->value.begin(), (*serviceIt)->value.end());
                    auto keyhash = dht::InfoHash::get((*serviceIt)->key);

                    m_dhtNode.put(keyhash, dht::Value(opendht_data));
                }
            }
        }
    }

    void DiscoveryMessageHandler::maintainZombieList(){
        int64_t currentTime = zclock_mono();

        for (auto it = m_zombieServices.begin(); it!=m_zombieServices.end(); it++){
            // 10 min timeout
            if ((currentTime - it->second) > 60*10*1000) {
                //std::cout << "Purge zombie from cache: " << it->first << std::endl;
                m_logger->info("Purge zombie from cache: {}", it->first);
                it = m_zombieServices.erase(it);
            }
        }
    }

    int DiscoveryMessageHandler::deregisterActor(const std::string& appName,
                                                 const std::string& actorName){

        std::string clientKeyBase = fmt::format("/{}/{}/",appName,actorName);
        std::string clientKeyLocal = clientKeyBase + m_macAddress;
        std::string clientKeyGlobal = clientKeyBase + m_hostAddress;

        std::vector<std::string> keysToBeErased{clientKeyBase, clientKeyLocal, clientKeyGlobal};

        m_logger->info("Unregister actor: ", clientKeyBase);

        int port = -1;
        if (m_clients.find(clientKeyBase)!=m_clients.end()){
            port = m_clients[clientKeyBase]->port;
        }

        for (auto it = keysToBeErased.begin(); it!=keysToBeErased.end(); it++){
            if (m_clients.find(*it)!=m_clients.end()){

                // erased elements
                int erased = m_clients.erase(*it);
                if (erased == 0) {
                    //std::cout << "Couldn't find actor to unregister: " << *it << std::endl;
                    m_logger->error("Couldn't find actor to unregister: {}", *it);
                }
            }
        }

        return port;
    }

    DiscoveryMessageHandler::~DiscoveryMessageHandler() {
        zpoller_destroy(&m_poller);
        zsock_destroy(&m_dhtUpdateSocket);
        zsock_destroy(&m_riapsSocket);
        sleep(1);

        for (auto it_client = m_clients.begin(); it_client!=m_clients.end(); it_client++){
            if (it_client->second->socket!=NULL) {
                zsock_destroy(&it_client->second->socket);
                it_client->second->socket=NULL;
            }
        }
        sleep(1);
    }

    std::tuple<std::string, std::string> DiscoveryMessageHandler::buildInsertKeyValuePair(
            const std::string&             appName,
            const std::string&             msgType,
            const riaps::discovery::Kind&  kind,
            const riaps::discovery::Scope& scope,
            const std::string&             host,
            const uint16_t                 port) {

        std::string key;
        key = "/" + appName
              + "/" + msgType
              + "/" + kindMap[kind];

        if (scope == riaps::discovery::Scope::LOCAL) {
            // hostid
            //auto hostid = gethostid();

            std::string mac_address = riaps::framework::Network::GetMacAddressStripped();

            key += mac_address;
        }

        std::string value = host + ":" + std::to_string(port);

        return std::make_tuple(key, value);
    }

    std::pair<std::string, std::string> DiscoveryMessageHandler::buildLookupKey(
            const std::string& appName,
            const std::string& msgType,
            const riaps::discovery::Kind& kind,
            const riaps::discovery::Scope& scope,
            const std::string& clientActorHost,
            const std::string& clientActorName,
            const std::string& clientInstanceName,
            const std::string& clientPortName) {


        std::string key;

        std::map<riaps::discovery::Kind, std::string> kindPairs = {
                {riaps::discovery::Kind::SUB, kindMap[riaps::discovery::Kind::PUB]},
                {riaps::discovery::Kind::CLT, kindMap[riaps::discovery::Kind::SRV]},
                {riaps::discovery::Kind::REQ, kindMap[riaps::discovery::Kind::REP]},
                {riaps::discovery::Kind::REP, kindMap[riaps::discovery::Kind::REQ]},
                {riaps::discovery::Kind::QRY, kindMap[riaps::discovery::Kind::ANS]},
                {riaps::discovery::Kind::ANS, kindMap[riaps::discovery::Kind::QRY]}
        };


        // TODO: Use fmt instead of the prehistoric string concat
        key =   "/" + appName
              + "/" + msgType
              + "/" + kindPairs[kind];

        //auto hostid = gethostid();

        std::string hostid = riaps::framework::Network::GetMacAddressStripped();


        if (scope == riaps::discovery::Scope::LOCAL) {
            key += hostid;
        }

        // TODO: Use fmt instead of the prehistoric string concat
        std::string client =   '/' + appName
                             + '/' + clientActorName
                             + '/' + clientActorHost
                             + '/' + clientInstanceName
                             + '/' + clientPortName;

        if (scope == riaps::discovery::Scope::LOCAL) {
            client = client + ":" + hostid;
        }

        return {key, client};
    }
}
//
// Created by istvan on 10/6/17.
//

#include <discoveryd/r_msghandler.h>
#include <framework/rfw_configuration.h>
#include <framework/rfw_network_interfaces.h>

namespace riaps{
    DiscoveryMessageHandler::DiscoveryMessageHandler(dht::DhtRunner &dhtNode, zsock_t** pipe)
        : _dhtNode(dhtNode),
          _pipe(*pipe),
          _serviceCheckPeriod(20000), // 20 sec in in msec.
          _zombieCheckPeriod(600000), // 10 min in msec
          _zombieKey("/zombies"),
          _terminated(false){

        _macAddress  = riaps::framework::Network::GetMacAddressStripped();
        _hostAddress = riaps::framework::Network::GetIPAddress();

    }

    bool DiscoveryMessageHandler::Init() {
        _dhtUpdateSocket = zsock_new_pull(DHT_ROUTER_CHANNEL);

        _riapsSocket = zsock_new_rep (riaps::framework::Configuration::GetDiscoveryServiceIpc().c_str());
        int lingerValue = 0;
        int sendtimeout = 0; // 0 - returns immediately with EAGAIN if the message cannot be sent
        zmq_setsockopt(_riapsSocket, ZMQ_LINGER, &lingerValue, sizeof(int));
        zmq_setsockopt(_riapsSocket, ZMQ_SNDTIMEO, &sendtimeout, sizeof(int));

        _poller = zpoller_new(_pipe, _riapsSocket, _dhtUpdateSocket, NULL);

        zsock_signal (_pipe, 0);

        _lastServiceCheckin = zclock_mono();
        _lastZombieCheck  = zclock_mono();

        // Get current zombies, and listen to new zombies
        _dhtNode.get(_zombieKey, [this](const std::vector<std::shared_ptr<dht::Value>> &values){
            return this->handleZombieUpdate(values);
        });

        // Subscribe for further zombies
        _dhtNode.listen(_zombieKey, [this](const std::vector<std::shared_ptr<dht::Value>> &values){
            return this->handleZombieUpdate(values);
        });
    }



    void DiscoveryMessageHandler::Run() {
        while (!_terminated){
            void *which = zpoller_wait(_poller, REGULAR_MAINTAIN_PERIOD);

            // Check services whether they are still alive.
            // Reregister the too old services (OpenDHT ValueType settings, it is 10 minutes by default)
            int64_t loopStartTime = zclock_mono();
            if ((loopStartTime-_lastServiceCheckin) > _serviceCheckPeriod){
                maintainRenewal();
            }

            // Check outdated zombies
            if ((loopStartTime-_lastZombieCheck) > _zombieCheckPeriod) {
                maintainZombieList();
            }

            // Handling messages from the caller (e.g.: $TERM$)
            if (which == _pipe) {
                handlePipeMessage();
            }
            else if (which == _dhtUpdateSocket){
                // Process the updated nodes
                zmsg_t* msgResponse = zmsg_recv(_dhtUpdateSocket);

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
                        handleDhtUpdate(msgProviderUpdate, _clientSubscriptions);

                    } else if (msgDhtUpdate.isProviderGet()) {
                        riaps::discovery::ProviderListGet::Reader msgProviderGet = msgDhtUpdate.getProviderGet();
                        handleDhtGet(msgProviderGet, _clients);
                    } else if (msgDhtUpdate.isZombieList()) {
                        auto zombieList = msgDhtUpdate.getZombieList();
                        for (int i =0; i< zombieList.size(); i++){
                            std::string currentZombie = zombieList[i];
                            _zombieServices[currentZombie] = zclock_mono();
                        }
                    } else if (msgDhtUpdate.isGroupUpdate()){
                        riaps::discovery::GroupUpdate::Reader msgGroupUpdate = msgDhtUpdate.getGroupUpdate();
                        handleDhtGroupUpdate(msgGroupUpdate);
                    }

                    zframe_destroy(&capnpMsgBody);
                    zmsg_destroy(&msgResponse);
                }
                catch(kj::Exception& e){
                    std::cout << "Couldn't deserialize message from DHT_ROUTER_SOCKET" << std::endl;
                    continue;

                }
            }

                // Handling messages from the RIAPS FW
                // Discovery service commands
            else if(which == _riapsSocket){
//                _terminated = handleRiapsMessages((zsock_t*)which,
//                                                 clients,
//                                                 serviceCheckins,
//                                                 clientSubscriptions,
//                                                 registeredListeners,
//                                                 host_address,
//                                                 mac_address,
//                                                 zombieServices,
//                                                 dhtNode);
                handleRiapsMessage();
            }
            else {


                //auto outdateds = maintain_servicecache(service_checkins);


            }
        }
    }

    void DiscoveryMessageHandler::handleRiapsMessage() {
        zmsg_t *riapsMessage = zmsg_recv(_riapsSocket);
        if (!riapsMessage) {
            std::cout << "No msg => interrupted" << std::endl;
            _terminated = true;
        } else {
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
        }
    }

    void DiscoveryMessageHandler::handleActorReg(riaps::discovery::ActorRegReq::Reader& msgActorReq) {

        std::string actorname = std::string(msgActorReq.getActorName().cStr());
        std::string appName   = std::string(msgActorReq.getAppName().cStr());

        std::string clientKeyBase = "/" + appName + '/' + actorname + "/";

        std::cout << "Register actor with PID - " << msgActorReq.getPid() << " : " << clientKeyBase << std::endl;

        auto registeredActorIt = _clients.find(clientKeyBase);
        bool isRegistered =  registeredActorIt!= _clients.end();

        // If the name of the actor is registered previously and it still run => Error
        bool isRunning = isRegistered && kill(registeredActorIt->second->pid, 0) == 0;
        //bool isRunning = false;

        // If the actor already registered and running
        if (isRunning) {
            std::cout << "Cannot register actor. This actor already registered (" << clientKeyBase << ")"
                      << std::endl;

            capnp::MallocMessageBuilder message;
            auto drepmsg = message.initRoot<riaps::discovery::DiscoRep>();
            auto arepmsg = drepmsg.initActorReg();
            arepmsg.setPort(0);
            arepmsg.setStatus(riaps::discovery::Status::ERR);


            auto serializedMessage = capnp::messageToFlatArray(message);

            zmsg_t *msg = zmsg_new();
            zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());
            zmsg_send(&msg, _riapsSocket);

        } else {

            // Purge the old instance
            if (isRegistered && !isRunning) {
                deregisterActor(appName, actorname);
            }

            // Open a new PAIR socket for actor communication
            zsock_t *actor_socket = zsock_new (ZMQ_PAIR);
            auto port = zsock_bind(actor_socket, "tcp://*:!");

            //_clients[clientKeyBase] = std::unique_ptr<actor_details_t>(new actor_details_t());
            _clients[clientKeyBase] =  std::make_shared<actor_details_t>();
            _clients[clientKeyBase] -> socket  = actor_socket;
            _clients[clientKeyBase] -> port    = port;
            _clients[clientKeyBase] -> pid     = msgActorReq.getPid();
            _clients[clientKeyBase] -> appName = appName;

            // Subscribe to groups
            if (_groupListeners.find(appName) == _groupListeners.end()) {
                std::string key = "/groups/"+appName;
                _groupListeners[appName] =
                        _dhtNode.listen(key, [](const std::vector<std::shared_ptr<dht::Value>> &values){
                            if (values.size() == 0) return true;

                            zsock_t *dhtNotificationSocket = zsock_new_push(DHT_ROUTER_CHANNEL);

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
                                std::cout << "[DHT] Group notifications sent to discovery service" << std::endl;
                            }

                            sleep(1);
                            zsock_destroy(&dhtNotificationSocket);
                            sleep(1);


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
            zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

            zmsg_send(&msg, _riapsSocket);

            // Use the same object, do not create another copy of actor_details;
            std::string clientKeyLocal = clientKeyBase + _macAddress;
            _clients[clientKeyLocal] = _clients[clientKeyBase];
            //_clients[clientKeyLocal] = std::unique_ptr<actor_details_t>(new actor_details_t());
            //_clients[clientKeyLocal]->port = port;
            //_clients[clientKeyLocal]->pid = msgActorReq.getPid();

            std::string clientKeyGlobal = clientKeyBase + _hostAddress;
            _clients[clientKeyGlobal] = _clients[clientKeyBase];
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
        if (_serviceCheckins.find(servicePid)!=_serviceCheckins.end()){
            for (auto& service : _serviceCheckins[servicePid]){
                std::string serviceAddress = service->value;
                std::vector<uint8_t> opendht_data(serviceAddress.begin(), serviceAddress.end());
                _dhtNode.put(_zombieKey, dht::Value(opendht_data));
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
        zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

        zmsg_send(&msg, _riapsSocket);
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

        std::cout << "Register service: " + kv_pair.first << std::endl;

        // New pid
        if (_serviceCheckins.find(servicePid) == _serviceCheckins.end()) {
            _serviceCheckins[servicePid] = std::vector<std::unique_ptr<service_checkins_t>>();
        }

        // Add PID - Service Details
        std::unique_ptr<service_checkins_t> newItem = std::unique_ptr<service_checkins_t>(new service_checkins_t());
        newItem->createdTime = zclock_mono();
        newItem->key         = kv_pair.first;
        newItem->value       = kv_pair.second;
        newItem->pid         = servicePid;
        _serviceCheckins[servicePid].push_back(std::move(newItem));

        // Convert the value to bytes
        std::vector<uint8_t> opendht_data(kv_pair.second.begin(), kv_pair.second.end());
        auto keyhash = dht::InfoHash::get(kv_pair.first);

        _dhtNode.put(keyhash, dht::Value(opendht_data));

        //Send response
        capnp::MallocMessageBuilder message;
        auto msg_discorep = message.initRoot<riaps::discovery::DiscoRep>();
        auto msg_servicereg_rep = msg_discorep.initServiceReg();

        msg_servicereg_rep.setStatus(riaps::discovery::Status::OK);

        auto serializedMessage = capnp::messageToFlatArray(message);

        zmsg_t *msg = zmsg_new();
        zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

        zmsg_send(&msg, _riapsSocket);
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
        auto current_client = std::unique_ptr<client_details_t>(new client_details_t());
        current_client->app_name      = path.getAppName();
        current_client->actor_host    = client.getActorHost();
        current_client->portname      = client.getPortName();
        current_client->actor_name    = client.getActorName();
        current_client->instance_name = client.getInstanceName();
        current_client->isLocal       = path.getScope() == riaps::discovery::Scope::LOCAL ? true : false;

        // Copy for the get callback
        client_details_t currentClientTmp(*current_client);

        // Now using just the discovery service to register the interested clients
        if (_clientSubscriptions.find(lookupkey.first) == _clientSubscriptions.end()) {
            // Nobody subscribed to this messagetype
            _clientSubscriptions[lookupkey.first] = std::vector<std::unique_ptr<client_details_t>>();
        }

        if (std::find(_clientSubscriptions[lookupkey.first].begin(),
                      _clientSubscriptions[lookupkey.first].end(),
                      current_client) == _clientSubscriptions[lookupkey.first].end()) {

            _clientSubscriptions[lookupkey.first].push_back(std::move(current_client));
        }

        dht::InfoHash lookupkeyhash = dht::InfoHash::get(lookupkey.first);

        std::cout << "Get: " + lookupkey.first << std::endl;

        //auto zombieServicesCopy = _zombieServices;
        _dhtNode.get(lookupkey.first,
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
                            std::cout << "Get results sent to discovery service" << std::endl;

                            sleep(1);
                            zsock_destroy(&notify_ractor_socket);
                            sleep(1);
                        } else {
                            std::cout << "Get has no results for query: " << lookupkey.first << std::endl;
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
        zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

        zmsg_send(&msg, _riapsSocket);

        // Start listener on this key if it hasn't been started yet.
        if (_registeredListeners.find(lookupkeyhash.toString()) == _registeredListeners.end()) {

            // Add listener to the added key
            _registeredListeners[lookupkeyhash.toString()] = _dhtNode.listen(lookupkeyhash,
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

                                                                                   std::cout
                                                                                           << "Changes sent to discovery service: "
                                                                                           << lookupkey.first
                                                                                           << std::endl;

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
        }

//        msgpack::sbuffer sbuf;
//        msgpack::pack(sbuf, groupDetails);
        std::string key = "/groups/"+appName;
//        dht::Blob b(sbuf.data(), sbuf.data()+sbuf.size());
//        _dhtNode.put(key, dht::Value(b));




        _dhtNode.put(key, dht::Value::pack(groupDetails));
        _dhtNode.get(key, [](const std::vector<std::shared_ptr<dht::Value>> &values){
            if (values.size() == 0) return true;

            zsock_t *dhtNotificationSocket = zsock_new_push(DHT_ROUTER_CHANNEL);

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
                std::cout << "[DHT] Group notifications sent to discovery service" << std::endl;
            }

            sleep(1);
            zsock_destroy(&dhtNotificationSocket);
            sleep(1);


            return true;
        });


        // Debug
        std::cout << "Component joined to group: "
                  << appName << "::"
                  << groupDetails.groupId.groupTypeId << "::"
                  << groupDetails.groupId.groupName   << std::endl;
        std::cout << "Group services: " << std::endl;
        for (auto& g : groupDetails.groupServices){
            std::cout << "\t- " << g.address << " " << g.messageType << std::endl;
        }

        //Send response
        capnp::MallocMessageBuilder repMessage;
        auto msgDiscoRep     = repMessage.initRoot<riaps::discovery::DiscoRep>();
        auto msgGroupJoinRep = msgDiscoRep.initGroupJoin();

        msgGroupJoinRep.setStatus(riaps::discovery::Status::OK);

        auto serializedMessage = capnp::messageToFlatArray(repMessage);

        zmsg_t *msg = zmsg_new();
        zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

        zmsg_send(&msg, _riapsSocket);
    }

    // Handle ZMQ messages, arriving on the zactor PIPE
    void DiscoveryMessageHandler::handlePipeMessage(){
        zmsg_t* msg = zmsg_recv(_pipe);
        if (!msg){
            std::cout << "No msg => interrupted" << std::endl;
            _terminated = true;
        }
        else {

            char *command = zmsg_popstr(msg);

            if (streq(command, "$TERM")) {
                std::cout << std::endl << "$TERMINATE arrived, discovery service is stopping..." << std::endl;
                _terminated = true;
            } else if (streq(command, CMD_JOIN)) {
                std::cout << "New peer on the network. Join()" << std::endl;
                bool has_more_msg = true;

                while (has_more_msg) {
                    char *newhost = zmsg_popstr(msg);
                    if (newhost) {
                        std::cout << "Join to: " << newhost << std::endl;
                        std::string str_newhost(newhost);
                        //dhtJoinToCluster(str_newhost, RIAPS_DHT_NODE_PORT, dhtNode);
                        _dhtNode.bootstrap(str_newhost, std::to_string(RIAPS_DHT_NODE_PORT));
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
                std::cout << "Service is considered as zombie: " << dhtResults[i].c_str() << std::endl;
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
                   const std::map<std::string, std::shared_ptr<actor_details_t>>& clients)
    {
        auto msgGetResults = msgProviderGet.getResults();

        for (int idx = 0; idx < msgGetResults.size(); idx++) {
            std::string resultEndpoint = std::string(msgGetResults[idx].cStr());

            if (_zombieServices.find(resultEndpoint)!=_zombieServices.end()) continue;

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
                std::cout << "Get results were sent to the client: " << clientKeyBase << std::endl;
            } else {
                zmsg_destroy(&msg);
                std::cout << "Get returned with values, but client has gone. " << std::endl;
            }


        }
    }

/// \brief Gets the provider updates from the OpenDHT (e.g.: new publisher registered) and sends a DiscoUpd message
///        to the interested clients.
/// \param msgProviderUpdate The capnp message from the OpenDHT Listen().
/// \param clientSubscriptions List of current key subscribtions.
/// \param clients  Holds the ZMQ sockets of the client actors.
    void DiscoveryMessageHandler::handleDhtUpdate(const riaps::discovery::ProviderListUpdate::Reader& msgProviderUpdate,
                      const std::map<std::string, std::vector<std::unique_ptr<client_details_t>>>& clientSubscriptions){

        std::string provider_key = std::string(msgProviderUpdate.getProviderpath().cStr());

        auto msg_newproviders = msgProviderUpdate.getNewvalues();

        // Look for services who may interested in the new provider
        if (clientSubscriptions.find(provider_key) != clientSubscriptions.end()) {
            for (auto &subscribedClient : clientSubscriptions.at(provider_key)) {
                for (int idx = 0; idx < msg_newproviders.size(); idx++) {
                    std::string new_provider_endpoint = std::string(msg_newproviders[idx].cStr());

                    // If the service marked as zombie
                    if (_zombieServices.find(new_provider_endpoint)!=_zombieServices.end()) continue;

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

                    std::string clientKeyBase = "/" + subscribedClient->app_name +
                                                "/" + subscribedClient->actor_name +
                                                "/";

                    std::cout << "Search for registered actor: " + clientKeyBase << std::endl;

                    // Python reference:
                    // TODO: Figure out, de we really need for this. I don't think so...
                    //if self.hostAddress != actorHost:
                    //continue

                    // If the client port saved before
                    if (_clients.find(clientKeyBase) != _clients.end()) {
                        const actor_details_t *clientSocket = _clients.at(clientKeyBase).get();

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

                            std::cout << "Port update sent to the client: " << clientKeyBase << std::endl;
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

        for (auto& client : _clients){
            if (client.second->appName == appName){
                if (sentCache.find(client.second->socket) != sentCache.end()) continue;

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

        // No active actor for this app, purge this listener
        if (!actorFound) {

        }
    }

    void DiscoveryMessageHandler::maintainRenewal(){

        std::vector<pid_t> toBeRemoved;
        for (auto it= _serviceCheckins.begin(); it!=_serviceCheckins.end(); it++){
            // Check pid, mark the removable pids
            // std::cout << "checking PID " << it->first << std::endl;
            if (!kill(it->first,0)==0){
                toBeRemoved.push_back(it->first);
            }
        }

        // Remove killed PIDs
        for (auto it = toBeRemoved.begin(); it!=toBeRemoved.end(); it++){
            std::cout << "Remove services with PID: " << *it << std::endl;

            // Put the service address into the zombies list in DHT
            for (auto serviceIt  = _serviceCheckins[*it].begin();
                 serviceIt != _serviceCheckins[*it].end();
                 serviceIt++) {

                // host:port
                std::string serviceAddress = (*serviceIt)->value;
                std::vector<uint8_t> opendht_data(serviceAddress.begin(), serviceAddress.end());
                _dhtNode.put(_zombieKey, dht::Value(opendht_data));
            }

            _serviceCheckins.erase(*it);
        }

        // Renew too old services
        int64_t now = zclock_mono();
        for (auto pidIt= _serviceCheckins.begin(); pidIt!=_serviceCheckins.end(); pidIt++){
            for(auto serviceIt = pidIt->second.begin(); serviceIt!=pidIt->second.end(); serviceIt++){

                // Renew
                if (now - (*serviceIt)->createdTime > (*serviceIt)->timeout){
                    (*serviceIt)->createdTime = now;

                    // Reput key-value
                    std::vector<uint8_t> opendht_data((*serviceIt)->value.begin(), (*serviceIt)->value.end());
                    auto keyhash = dht::InfoHash::get((*serviceIt)->key);

                    _dhtNode.put(keyhash, dht::Value(opendht_data));
                }
            }
        }
    }

    void DiscoveryMessageHandler::maintainZombieList(){
        int64_t currentTime = zclock_mono();

        for (auto it = _zombieServices.begin(); it!=_zombieServices.end(); it++){
            // 10 min timeout
            if ((currentTime - it->second) > 60*10*1000) {
                std::cout << "Purge zombie from cache: " << it->first << std::endl;
                it = _zombieServices.erase(it);
            }
        }
    }

    int DiscoveryMessageHandler::deregisterActor(const std::string& appName,
                                                 const std::string& actorName){

        std::string clientKeyBase = "/" + appName + '/' + actorName + "/";
        std::string clientKeyLocal = clientKeyBase + _macAddress;
        std::string clientKeyGlobal = clientKeyBase + _hostAddress;

        std::vector<std::string> keysToBeErased{clientKeyBase, clientKeyLocal, clientKeyGlobal};

        std::cout << "Unregister actor: " << clientKeyBase << std::endl;

        int port = -1;
        if (_clients.find(clientKeyBase)!=_clients.end()){
            port = _clients[clientKeyBase]->port;
        }

        for (auto it = keysToBeErased.begin(); it!=keysToBeErased.end(); it++){
            if (_clients.find(*it)!=_clients.end()){

                // erased elements
                int erased = _clients.erase(*it);
                if (erased == 0) {
                    std::cout << "Couldn't find actor to unregister: " << *it << std::endl;
                }
            }
        }

        return port;
    }

    DiscoveryMessageHandler::~DiscoveryMessageHandler() {
        zpoller_destroy(&_poller);
        zsock_destroy(&_dhtUpdateSocket);
        zsock_destroy(&_riapsSocket);
        sleep(1);

        for (auto it_client = _clients.begin(); it_client!=_clients.end(); it_client++){
            if (it_client->second->socket!=NULL) {
                zsock_destroy(&it_client->second->socket);
                it_client->second->socket=NULL;
            }
        }
        sleep(1);
    }

    std::pair<std::string, std::string> DiscoveryMessageHandler::buildInsertKeyValuePair(
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

        return std::pair<std::string, std::string>(key, value);
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
                {riaps::discovery::Kind::REP, kindMap[riaps::discovery::Kind::REQ]}};

        key = "/" + appName
              + "/" + msgType
              + "/" + kindPairs[kind];

        //auto hostid = gethostid();

        std::string hostid = riaps::framework::Network::GetMacAddressStripped();

        if (scope == riaps::discovery::Scope::LOCAL) {
            key += hostid;
        }

        std::string client = '/' + appName
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
#include "discoveryd/r_riaps_actor.h"
#include "componentmodel/r_network_interfaces.h"
#include "discoveryd/r_odht.h"
#include <vector>
#include <map>

#define REGULAR_MAINTAIN_PERIOD 3000 //msec

#define DHT_ROUTER_CHANNEL "ipc:///tmp/dhtrouterchannel"

void
riaps_actor (zsock_t *pipe, void *args)
{
    std::string mac_address = GetMacAddressStripped();
    std::string host_address = GetIPAddress();

    std::cout << "Discovery service is starting, network interface: " << std::endl
              << " * " << host_address << std::endl
              << " * " << mac_address  << std::endl;


    std::cout << "Start DHT node." << std::endl;
    dht::DhtRunner dht_node;

    // Launch a dht node on a new thread, using a
    // generated RSA key pair, and listen on port 4222.

    dht_node.run(RIAPS_DHT_NODE_PORT, dht::crypto::generateIdentity(), true);

    std::cout << "DHT node started." <<std::endl;

    std::srand(std::time(0));

    zsock_t* dht_router_socket = zsock_new_pull(DHT_ROUTER_CHANNEL);
    assert(dht_router_socket);

    // Response socket for incoming messages from RIAPS Components
    zsock_t * riaps_socket = zsock_new_rep (DISCOVERY_SERVICE_IPC(mac_address));
    assert(riaps_socket);

    // Socket for OpenDHT communication.
    zpoller_t* poller = zpoller_new(pipe, riaps_socket, dht_router_socket, NULL);
    assert(poller);

    bool terminated = false;
    zsock_signal (pipe, 0);

    // Store the last checkins of the registered services
    // If there was no checkin in SERVICE_TIMEOUT ms then, remove the service from consul
    //std::map<std::string, int64_t> service_checkins;

    // Stores pair sockets for actor communication
    std::map<std::string, std::unique_ptr<actor_details>> clients;

    // Client subscriptions to messageTypes
    std::map<std::string, std::vector<std::unique_ptr<client_details>>> clientSubscriptions;

    // Registered OpenDHT listeners. Every key can be registered only once.
    std::map<std::string, std::future<size_t>> registeredListeners;

    while (!terminated){
        void *which = zpoller_wait(poller, REGULAR_MAINTAIN_PERIOD);

        // Handling messages from the caller (e.g.: $TERM$)
        if (which == pipe) {
            zmsg_t* msg = zmsg_recv(which);
            if (!msg){
                std::cout << "No msg => interrupted" << std::endl;
                break;
            }

            char* command = zmsg_popstr(msg);

            if (streq(command, "$TERM")){
                std::cout << std::endl << "$TERMINATE arrived, discovery service is stopping..." << std::endl;
                terminated = true;
            }
            else if (streq(command, "JOIN")) {
                std::cout << "New peer on the network. Join()" << std::endl;
                bool has_more_msg = true;

                while (has_more_msg){
                    char* newhost = zmsg_popstr(msg);
                    if (newhost){
                        std::cout << "Join to: " << newhost << std::endl;
                        std::string str_newhost(newhost);
                        dhtJoinToCluster(str_newhost, RIAPS_DHT_NODE_PORT, dht_node);
                        zstr_free(&newhost);
                    } else{
                        has_more_msg = false;
                    }
                }
            }

            if (command){
                zstr_free(&command);
            }
            zmsg_destroy(&msg);
        }
        else if (which == dht_router_socket){
            // Process the updated nodes
            zmsg_t* msgResponse = zmsg_recv(dht_router_socket);

            zframe_t* capnpMsgBody = zmsg_pop(msgResponse);
            size_t    size = zframe_size(capnpMsgBody);
            byte*     data = zframe_data(capnpMsgBody);


            auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

            capnp::FlatArrayMessageReader reader(capnp_data);
            auto msgProviderlistPush = reader.getRoot<ProviderListPush>();

            // If update
            if (msgProviderlistPush.isProviderUpdate()) {
                ProviderListUpdate::Reader msgProviderUpdate = msgProviderlistPush.getProviderUpdate();
                handleUpdate(msgProviderUpdate, clientSubscriptions, clients);

            } else if (msgProviderlistPush.isProviderGet()){
                ProviderListGet::Reader msgProviderGet = msgProviderlistPush.getProviderGet();
                handleGet(msgProviderGet, clients);
            }

            zframe_destroy(&capnpMsgBody);
            zmsg_destroy(&msgResponse);
        }

        // Handling messages from the RIAPS FW
        // Discovery service commands
        else if(which == riaps_socket){
            zmsg_t* msg = zmsg_recv(which);
            
            if (!msg){
                std::cout << "No msg => interrupted" << std::endl;
                break;
            }

            zframe_t* capnp_msgbody = zmsg_pop(msg);
            size_t    size = zframe_size(capnp_msgbody);
            byte*     data = zframe_data(capnp_msgbody);

            auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

            capnp::FlatArrayMessageReader reader(capnp_data);
            auto msg_discoreq= reader.getRoot<DiscoReq>();

            //zsys_info("Message arrived: %s (%s)", "DiscoReq", msg_discoreq.which());

            // Register actor
            if (msg_discoreq.isActorReg()) {
                auto msg_actorreq = msg_discoreq.getActorReg();
                std::string actorname    = std::string(msg_actorreq.getActorName());
                std::string appname      = std::string(msg_actorreq.getAppName());

                std::string clientKeyBase = "/" + appname + '/' + actorname + "/";

                std::cout << "Register actor: " << clientKeyBase << std::endl;

                // If the actor already registered
                if (clients.find(clientKeyBase)!=clients.end()) {
                    std::cout << "Cannot register actor. This actor already registered (" << clientKeyBase << ")" << std::endl;

                    capnp::MallocMessageBuilder message;
                    auto drepmsg = message.initRoot<DiscoRep>();
                    auto arepmsg = drepmsg.initActorReg();

                    arepmsg.setPort(0);
                    arepmsg.setStatus(Status::ERR);

                    auto serializedMessage = capnp::messageToFlatArray(message);

                    zmsg_t* msg = zmsg_new();
                    zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

                    zmsg_send(&msg, riaps_socket);

                } else{
                    // Open a new PAIR socket for actor communication
                    zsock_t * actor_socket = zsock_new (ZMQ_PAIR);
                    auto port = zsock_bind(actor_socket, "tcp://*:!");

                    clients[clientKeyBase] = std::unique_ptr<actor_details>(new actor_details());
                    clients[clientKeyBase]->socket     = actor_socket;
                    clients[clientKeyBase]->port = port;

                    // Create and send the Response
                    capnp::MallocMessageBuilder message;
                    auto drepmsg = message.initRoot<DiscoRep>();
                    auto arepmsg = drepmsg.initActorReg();

                    arepmsg.setPort(port);
                    arepmsg.setStatus(Status::OK);

                    auto serializedMessage = capnp::messageToFlatArray(message);

                    zmsg_t* msg = zmsg_new();
                    zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

                    zmsg_send(&msg, riaps_socket);

                    std::string clientKeyLocal = clientKeyBase + mac_address;
                    clients[clientKeyLocal] = std::unique_ptr<actor_details>(new actor_details());
                    clients[clientKeyLocal]->port = port;

                    std::string clientKeyGlobal = clientKeyBase + host_address;
                    clients[clientKeyGlobal] = std::unique_ptr<actor_details>(new actor_details());
                    clients[clientKeyGlobal]->port = port;
                }
            } else if (msg_discoreq.isServiceReg()){
                auto msg_servicereg_req = msg_discoreq.getServiceReg();
                auto msg_path           = msg_servicereg_req.getPath();
                auto msg_sock           = msg_servicereg_req.getSocket();

                auto kv_pair = buildInsertKeyValuePair(msg_path.getAppName(),
                                                       msg_path.getMsgType(),
                                                       msg_path.getKind(),
                                                       msg_path.getScope(),
                                                       msg_sock.getHost(),
                                                       msg_sock.getPort());


                std::cout << "Register service: " + kv_pair.first << std::endl;

                // Convert the value to bytes
                std::vector<uint8_t> opendht_data(kv_pair.second.begin(), kv_pair.second.end());
                auto keyhash = dht::InfoHash::get(kv_pair.first);


                dht_node.put(keyhash, dht::Value(opendht_data));

                //Send response
                capnp::MallocMessageBuilder message;
                auto msg_discorep       = message.initRoot<DiscoRep>();
                auto msg_servicereg_rep = msg_discorep.initServiceReg();

                msg_servicereg_rep.setStatus(Status::OK);

                auto serializedMessage = capnp::messageToFlatArray(message);

                zmsg_t* msg = zmsg_new();
                zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

                zmsg_send(&msg, riaps_socket);

            } else if(msg_discoreq.isServiceLookup()){
                auto msg_servicelookup = msg_discoreq.getServiceLookup();

                auto client = msg_servicelookup.getClient();
                auto path   = msg_servicelookup.getPath();

                // Key   -> /appname/msgType/kind
                // Value -> /appname/clientactorname/clienthost/clientinstancename/clientportname
                // The "value" is interested in "key"
                auto lookupkey =
                           buildLookupKey(path.getAppName()        ,
                                          path.getMsgType()        ,
                                          path.getKind()           ,
                                          path.getScope()          ,
                                          client.getActorHost()    ,
                                          client.getActorName()    ,
                                          client.getInstanceName() ,
                                          client.getPortName()     );

                // This client is interested in this kind of messages. Register it.
                auto current_client           = std::unique_ptr<client_details>(new client_details());
                current_client->app_name      = path.getAppName();
                current_client->actor_host    = client.getActorHost();
                current_client->portname      = client.getPortName();
                current_client->actor_name    = client.getActorName();
                current_client->instance_name = client.getInstanceName();
                current_client->isLocal       = path.getScope() == Scope::LOCAL?true:false;

                // Copy for the get callback
                client_details currentClientTmp(*current_client);


                // Now using just the discovery service to register the interested clients
                if (clientSubscriptions.find(lookupkey.first) == clientSubscriptions.end()){
                    // Nobody subscribed to this messagetype
                    clientSubscriptions[lookupkey.first] = std::vector<std::unique_ptr<client_details>>();
                }

                if (std::find(clientSubscriptions[lookupkey.first].begin(),
                              clientSubscriptions[lookupkey.first].end()  ,
                              current_client) == clientSubscriptions[lookupkey.first].end()){

                    clientSubscriptions[lookupkey.first].push_back(std::move(current_client));
                }

                dht::InfoHash lookupkeyhash = dht::InfoHash::get(lookupkey.first);

                std::cout << "Get: " + lookupkey.first << std::endl;


                dht_node.get(lookupkey.first, [currentClientTmp, lookupkey](const std::vector<std::shared_ptr<dht::Value>>& values){
                    // Done Callback

                    std::vector<std::string> dht_lookup_results;
                    for (const auto& value :values ){
                        std::string result = std::string(value->data.begin(), value->data.end());
                        dht_lookup_results.push_back(result);
                    }

                    if (dht_lookup_results.size()>0) {

                        zsock_t *notify_ractor_socket = zsock_new_push(DHT_ROUTER_CHANNEL);

                        capnp::MallocMessageBuilder message;
                        auto msg_providerlistpush = message.initRoot<ProviderListPush>();
                        auto msg_providerget = msg_providerlistpush.initProviderGet();

                        auto msg_path = msg_providerget.initPath();

                        Scope scope = currentClientTmp.isLocal?Scope::LOCAL:Scope::GLOBAL;
                        std::string app_name = currentClientTmp.app_name;

                        msg_path.setScope(scope);
                        msg_path.setAppName(currentClientTmp.app_name);

                        auto msg_client = msg_providerget.initClient();

                        msg_client.setActorName(currentClientTmp.actor_name);
                        msg_client.setPortName(currentClientTmp.portname);
                        msg_client.setActorHost(currentClientTmp.actor_host);
                        msg_client.setInstanceName(currentClientTmp.instance_name);

                        auto number_of_results = dht_lookup_results.size();
                        ::capnp::List<::capnp::Text>::Builder get_results = msg_providerget.initResults(number_of_results);

                        int provider_index = 0;
                        for (std::string provider : dht_lookup_results) {
                            char* c = (char*)provider.c_str();
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
                    }
                    else {
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



                //wait();

                //std::cout << "Get results: ";

                //for (auto r : dht_lookup_results){
                //    std::cout << r << ", ";
                //}

               // std::endl(std::cout);



                //Send response
                capnp::MallocMessageBuilder message;
                auto msg_discorep       = message.initRoot<DiscoRep>();
                auto msg_service_lookup_rep = msg_discorep.initServiceLookup();

                msg_service_lookup_rep.setStatus(Status::OK);

                //auto number_of_clients =dht_lookup_results.size();

                auto number_of_clients = 0;
                auto sockets = msg_service_lookup_rep.initSockets(number_of_clients);


                //for (int i = 0; i<number_of_clients; i++){
                    //auto dht_result = dht_lookup_results[i];
                    //auto pos = dht_result.find_first_of(':');
                    //if (pos==std::string::npos) continue;

                    //auto host = std::string(dht_result.begin(), dht_result.begin()+pos);
                    //auto port = std::string(dht_result.begin()+pos+1, dht_result.end());

                    //sockets[i].setHost(host);
                    //sockets[i].setPort(std::stoi(port));

                    //std::cout << "Added: " << host << " " << std::stoi(port) << std::endl;
                //}

               // msg_service_lookup_rep.setSockets()

                //msg_servicereg_rep.setStatus(Status::OK);

                auto serializedMessage = capnp::messageToFlatArray(message);

                zmsg_t* msg = zmsg_new();
                zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

                zmsg_send(&msg, riaps_socket);

                // Start listener on this key if it hasn't been started yet.
                if (registeredListeners.find(lookupkeyhash.toString())==registeredListeners.end()) {

                    // Add listener to the added key
                    registeredListeners[lookupkeyhash.toString()] = dht_node.listen(lookupkeyhash,
                        [lookupkey](const std::vector<std::shared_ptr<dht::Value>> &values) {


                            //std::cout << "Value changed, send back the changes to the discovery service"<< std::endl;

                            zsock_t *notify_ractor_socket = zsock_new_push(DHT_ROUTER_CHANNEL);

                            capnp::MallocMessageBuilder message;


                            auto msg_providerlist_push = message.initRoot<ProviderListPush>();
                            auto msg_provider_update   = msg_providerlist_push.initProviderUpdate();
                            msg_provider_update.setProviderpath(lookupkey.first);

                            std::vector<std::string> update_results;
                            for (const auto &value :values) {
                                std::string result = std::string(value->data.begin(),
                                                                 value->data.end());
                                update_results.push_back(result);
                            }

                            auto number_of_providers = update_results.size();
                            ::capnp::List<::capnp::Text>::Builder msg_providers = msg_provider_update.initNewvalues(
                                    number_of_providers);

                            int provider_index = 0;
                            for (std::string provider : update_results) {
                                ::capnp::Text::Builder b((char *) provider.c_str());
                                msg_providers.set(provider_index++, b.asString());
                            }

                            auto serializedMessage = capnp::messageToFlatArray(message);

                            zmsg_t *msg = zmsg_new();
                            zmsg_pushmem(msg, serializedMessage.asBytes().begin(),
                                         serializedMessage.asBytes().size());

                            zmsg_send(&msg, notify_ractor_socket);

                            std::cout << "Changes sent to discovery service: " << lookupkey.first << std::endl;

                            sleep(1);
                            zsock_destroy(&notify_ractor_socket);
                            sleep(1);

                            return true; // keep listening
                        }
);
                }

                // Subscribe, if new provider arrives

                // -- Register with OpenDHT --

                //std::string client_subscribe_key = lookupkey.first + "_clients";
                //std::vector<uint8_t> opendht_client_subscribe_data(client_subscribe_key.begin(), client_subscribe_key.end());
                //auto keyhash = dht::InfoHash::get(client_subscribe_key);
                //dht_node.put(keyhash, dht::Value(opendht_client_subscribe_data));


            }




            //capnp::initMessageBuilderFromFlatArrayCopy(reader.getRoot(), message);

            //capnp::MallocMessageBuilder message;
            //capnp::initMessageBuilderFromFlatArrayCopy(kjptr, message);
            /*
            char* command = zmsg_popstr(msg);
            
            if (command){

                if (streq(command, CMD_DISC_GETSERVICE_BY_NAME_POLL_ASYNC)) {
                    //zframe_t* flat_msgbody = zmsg_pop(msg);
                    //byte* framedata = zframe_data(flat_msgbody);
                    //auto m = Getmsg_getservice_poll_request(framedata);

                    //auto s = m->service_name()->str();
                    //auto r = m->reply_id()->str();


                    //int i =5;
                }

                    // TODO: only handle_command should be called, without ifs
                if (streq(command, CMD_DISC_REGISTER_SERVICE)) {
                    bool handle_result = handle_command(command, msg, riaps_socket, async_service_poller);
                    assert(handle_result);
                }
                else if (streq(command, CMD_DISC_DEREGISTER_SERVICE)){
                    bool handle_result = handle_command(command, msg, riaps_socket, async_service_poller);
                    assert(handle_result);
                }
                else if (streq(command, CMD_DISC_GET_SERVICES)) {
                    bool handle_result = handle_command(command, msg, riaps_socket, async_service_poller);
                    assert(handle_result);
                }
                else if (streq(command, CMD_DISC_GETSERVICE_BY_NAME)) {
                    bool handle_result = handle_command(command, msg, riaps_socket, async_service_poller);
                    assert(handle_result);
                }
                else if (streq(command, CMD_DISC_GETSERVICE_BY_NAME_ASYNC)){
                    bool handle_result = handle_command(command, msg, riaps_socket, async_service_poller);
                    assert(handle_result);
                }
                else if(streq(command, CMD_DISC_REGISTER_NODE)){
                    bool handle_result = handle_command(command, msg, riaps_socket, async_service_poller);
                    assert(handle_result);
                }


                else if (streq(command, CMD_DISC_REGISTER_ACTOR)){
                    char* actorname = zmsg_popstr(msg);
                    if (actorname){
                        disc_registeractor(hostname, actorname);
                        free(actorname);
                        zstr_send(riaps_socket, "OK");
                    }
                }
                else if(streq(command, CMD_DISC_DEREGISTER_ACTOR)){
                    char* actorname = zmsg_popstr(msg);
                    if (actorname){
                        disc_deregisteractor(hostname, actorname);

                        free(actorname);

                        zstr_send(riaps_socket, "OK");
                    }
                }
                else if(streq(command, CMD_DISC_PING)){

                    std::cout << "Ping arrived" << std::endl;

                    char* service_name = zmsg_popstr(msg);
                    if (service_name){

                        //std::cout << "   " << service_name << std::endl;
                        int64_t time = zclock_mono();
                        service_checkins[service_name] = time;

                        // remove outdated services from the cache and from the discovery service
                        auto outdateds = maintain_servicecache(service_checkins);

                        for (auto outdated : outdateds){
                            std::cout << outdated << ";" ;
                        }

                        std::cout << std::endl;

                        free(service_name);
                        //zstr_send("OK", riaps_socket);
                        zstr_send(riaps_socket, "OK");
                    }
                }
                else {

                }

                free(command);
            }*/
        }
        else {
            //std::cout << "Regular maintain, cannot stop: " << terminated <<std::endl;

            //zmsg_t* msg = zmsg_recv(which);
            //if (msg){
            //    std::cout << "HEYY MSG!" << std::endl;
            //    break;
            //}

            //auto outdateds = maintain_servicecache(service_checkins);

            //for (auto outdated : outdateds){
            //    std::cout << outdated << ";" ;
            //}

            //std::cout << std::endl;
        }
    }

    //dht_node.setOnStatusChanged()

    dht_node.join();

    for (auto it_client = clients.begin(); it_client!=clients.end(); it_client++){
        if (it_client->second->socket!=NULL) {
            zsock_destroy(&it_client->second->socket);
            it_client->second->socket=NULL;
        }
    }


    zpoller_destroy(&poller);
    //zactor_destroy(&async_service_poller);
    zsock_destroy(&riaps_socket);
    zsock_destroy(&dht_router_socket);

    sleep(1);
}

/*bool _client_details::operator==(const client_details &rhs) {
        return app_name      == rhs.app_name      &&
               actor_name    == rhs.actor_name    &&
               actor_host    == rhs.actor_host    &&
               instance_name == rhs.instance_name &&
               portname      == rhs.portname      &&
               isLocal       == rhs.isLocal;
}*/


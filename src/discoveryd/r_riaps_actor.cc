#include <discoveryd/r_riaps_actor.h>
#include <discoveryd/r_discoveryd_commands.h>
#include <discoveryd/r_odht.h>
#include <framework/rfw_configuration.h>
#include <framework/rfw_network_interfaces.h>

#include <vector>
#include <map>

#define REGULAR_MAINTAIN_PERIOD 3000 //msec

//extern "C" std::string riaps::framework::Network::GetIPAddress(const std::string& ifacename);



void
riaps_actor (zsock_t *pipe, void *args)
{
    std::string mac_address = riaps::framework::Network::GetMacAddressStripped();
    std::string host_address = riaps::framework::Network::GetIPAddress();


    std::cout //<< "Discovery service is starting, network interface: " << std::endl
              << " * " << host_address << std::endl
              << " * " << mac_address  << std::endl;


    std::cout << "Start DHT node." << std::endl;
    dht::DhtRunner dhtNode;

    // Launch a dht node on a new thread, using a
    // generated RSA key pair, and listen on port 4222.

    //dht_node.run(RIAPS_DHT_NODE_PORT, dht::crypto::generateIdentity(), true);
    dhtNode.run(RIAPS_DHT_NODE_PORT, {}, true);


    std::cout << "DHT node started." <<std::endl;

    std::srand(std::time(0));

    zsock_t* dhtUpdateSocket = zsock_new_pull(DHT_ROUTER_CHANNEL);
    assert(dhtUpdateSocket);

    // Response socket for incoming messages from RIAPS Components
    zsock_t * riaps_socket = zsock_new_rep (riaps::framework::Configuration::GetDiscoveryServiceIpc().c_str());
    assert(riaps_socket);
    int lingerValue = 0;
    int sendtimeout = 0; // 0 - returns immediately with EAGAIN if the message cannot be sent
    zmq_setsockopt(riaps_socket, ZMQ_LINGER, &lingerValue, sizeof(int));
    zmq_setsockopt(riaps_socket, ZMQ_SNDTIMEO, &sendtimeout, sizeof(int));

    // Socket for OpenDHT communication.
    zpoller_t* poller = zpoller_new(pipe, riaps_socket, dhtUpdateSocket, NULL);
    assert(poller);

    bool terminated = false;
    zsock_signal (pipe, 0);

    // Store the last checkins of the registered services
    // If there was no checkin in SERVICE_TIMEOUT ms then, remove the service from consul
    //std::map<std::string, int64_t> service_checkins;

    // Stores pair sockets for actor communication
    std::map<std::string, std::unique_ptr<actor_details_t>> clients;

    // Stores addresses of zombie services
    // A service is zombie, if the related socket is not able to respond, but it is still in the DHT
    // The int64 argument is a timestamp. Old zombies are removed from the set after 10 minutes.
    std::map<std::string, int64_t> zombieServices;

    // Client subscriptions to messageTypes
    std::map<std::string, std::vector<std::unique_ptr<client_details_t>>> clientSubscriptions;

    // Registered OpenDHT listeners. Every key can be registered only once.
    std::map<std::string, std::future<size_t>> registeredListeners;

    // Registered services, with PID-s. We are using this local cache for renew services in the OpenDHT.
    // Checking the registered services in every 20th seconds.
    std::map<pid_t, std::vector<std::unique_ptr<service_checkins_t>>> serviceCheckins;
    int64_t lastServiceCheckin = zclock_mono();
    const uint16_t serviceCheckPeriod = 20000;  // 20 sec in in msec.
    int64_t  lastZombieCheck  = zclock_mono();
    const uint16_t zombieCheckPeriod  = 600000; // 10 min in msec

    // Get current zombies, and listen to new zombies

    dhtNode.get(zombieKey, handleZombieUpdate);

    // Subscribe for further zombies in the future
    dhtNode.listen(zombieKey, handleZombieUpdate);

    while (!terminated){
        void *which = zpoller_wait(poller, REGULAR_MAINTAIN_PERIOD);

//        if (zombieServices.size()>0){
//            std::cout << "Stored zombies: " << std::endl;
//            for (auto& z : zombieServices){
//                std::cout << z.first << std::endl;
//            }
//        }

        // Check services whether they are still alive.
        // Reregister the too old services (OpenDHT ValueType settings, it is 10 minutes by default)
        int64_t loopStartTime = zclock_mono();
        if ((loopStartTime-lastServiceCheckin) > serviceCheckPeriod){
            maintainRenewal(serviceCheckins, dhtNode);
        }

        // Check outdated zombies
        if ((loopStartTime-lastZombieCheck) > zombieCheckPeriod) {
            maintainZombieList(zombieServices);
        }

        // Handling messages from the caller (e.g.: $TERM$)
        if (which == pipe) {
            terminated = handlePipeMessage((zsock_t*)which, dhtNode);
        }
        else if (which == dhtUpdateSocket){
            // Process the updated nodes
            zmsg_t* msgResponse = zmsg_recv(dhtUpdateSocket);

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
                    handleUpdate(msgProviderUpdate, clientSubscriptions, clients);

                } else if (msgDhtUpdate.isProviderGet()) {
                    riaps::discovery::ProviderListGet::Reader msgProviderGet = msgDhtUpdate.getProviderGet();
                    handleGet(msgProviderGet, clients);
                } else if (msgDhtUpdate.isZombieList()) {
                    auto zombieList = msgDhtUpdate.getZombieList();
                    int s = zombieList.size();
                    for (int i =0; i< zombieList.size(); i++){
                        std::string currentZombie = zombieList[i];
                        zombieServices[currentZombie] = zclock_mono();
                        std::cout << "Stored zombies: " << std::endl;
                        for (auto& z : zombieServices){
                            std::cout << z.first << std::endl;
                        }
                    }
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
        else if(which == riaps_socket){
            terminated = handleRiapsMessages((zsock_t*)which,
                                             clients,
                                             serviceCheckins,
                                             clientSubscriptions,
                                             registeredListeners,
                                             host_address,
                                             mac_address,
                                             zombieServices,
                                             dhtNode);
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

    dhtNode.join();

    for (auto it_client = clients.begin(); it_client!=clients.end(); it_client++){
        if (it_client->second->socket!=NULL) {
            zsock_destroy(&it_client->second->socket);
            it_client->second->socket=NULL;
        }
    }


    zpoller_destroy(&poller);
    //zactor_destroy(&async_service_poller);
    zsock_destroy(&riaps_socket);
    zsock_destroy(&dhtUpdateSocket);

    sleep(1);
}

int deregisterActor(const std::string& appName,
                     const std::string& actorName,
                     const std::string& macAddress,
                     const std::string& hostAddress,
                     std::map<std::string, std::unique_ptr<actor_details_t>>& clients){

    std::string clientKeyBase = "/" + appName + '/' + actorName + "/";
    std::string clientKeyLocal = clientKeyBase + macAddress;
    std::string clientKeyGlobal = clientKeyBase + hostAddress;

    std::vector<std::string> keysToBeErased{clientKeyBase, clientKeyLocal, clientKeyGlobal};

    std::cout << "Unregister actor: " << clientKeyBase << std::endl;

    int port = -1;
    if (clients.find(clientKeyBase)!=clients.end()){
        port = clients[clientKeyBase]->port;
    }

    for (auto it = keysToBeErased.begin(); it!=keysToBeErased.end(); it++){
        if (clients.find(*it)!=clients.end()){

            // erased elements
            int erased = clients.erase(*it);
            if (erased == 0) {
                std::cout << "Couldn't find actor to unregister: " << *it << std::endl;
            }
        }
    }

    return port;
}


void maintainRenewal(std::map<pid_t, std::vector<std::unique_ptr<service_checkins_t>>>& serviceCheckins, dht::DhtRunner& dhtNode){

    std::vector<pid_t> toBeRemoved;
    for (auto it= serviceCheckins.begin(); it!=serviceCheckins.end(); it++){
        // Check pid, mark the removable pids
        std::cout << "checking PID " << it->first << std::endl;
        if (!kill(it->first,0)==0){
            toBeRemoved.push_back(it->first);
        }
    }

    // Remove killed PIDs
    for (auto it = toBeRemoved.begin(); it!=toBeRemoved.end(); it++){
        std::cout << "Remove services with PID: " << *it << std::endl;

        // Put the service address into the zombies list in DHT
        for (auto serviceIt  = serviceCheckins[*it].begin();
                  serviceIt != serviceCheckins[*it].end();
                  serviceIt++) {

            // host:port
            std::string serviceAddress = (*serviceIt)->value;
            std::vector<uint8_t> opendht_data(serviceAddress.begin(), serviceAddress.end());
            dhtNode.put(zombieKey, dht::Value(opendht_data));
        }

        serviceCheckins.erase(*it);
    }

    // Renew too old services
    int64_t now = zclock_mono();
    for (auto pidIt= serviceCheckins.begin(); pidIt!=serviceCheckins.end(); pidIt++){
        for(auto serviceIt = pidIt->second.begin(); serviceIt!=pidIt->second.end(); serviceIt++){

            // Renew
            if (now - (*serviceIt)->createdTime > (*serviceIt)->timeout){
                (*serviceIt)->createdTime = now;

                // Reput key-value
                std::vector<uint8_t> opendht_data((*serviceIt)->value.begin(), (*serviceIt)->value.end());
                auto keyhash = dht::InfoHash::get((*serviceIt)->key);

                dhtNode.put(keyhash, dht::Value(opendht_data));
            }
        }
    }
}

bool handleZombieUpdate(const std::vector<std::shared_ptr<dht::Value>> &values){
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

void maintainZombieList(std::map<std::string, int64_t>& zombieList){
    int64_t currentTime = zclock_mono();

    for (auto it = zombieList.begin(); it!=zombieList.end(); it++){
        if ((currentTime - it->second) > 600000) {
            // TODO: put it back
            //it = zombieList.erase(it);
        }
    }
}

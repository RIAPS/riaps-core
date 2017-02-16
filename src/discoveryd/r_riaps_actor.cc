#include "discoveryd/r_riaps_actor.h"
#include "componentmodel/r_network_interfaces.h"
#include "discoveryd/r_discoveryd_commands.h"
#include "discoveryd/r_odht.h"
#include <vector>
#include <map>

#define REGULAR_MAINTAIN_PERIOD 3000 //msec



void
riaps_actor (zsock_t *pipe, void *args)
{
    std::string mac_address = GetMacAddressStripped();
    std::string host_address = GetIPAddress();

    std::cout << "Discovery service is starting, network interface: " << std::endl
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
    std::map<std::string, std::unique_ptr<actor_details_t>> clients;

    // Client subscriptions to messageTypes
    std::map<std::string, std::vector<std::unique_ptr<client_details_t>>> clientSubscriptions;

    // Registered OpenDHT listeners. Every key can be registered only once.
    std::map<std::string, std::future<size_t>> registeredListeners;

    // Registered services, with PID-s. We are using this local cache for renew services in the OpenDHT.
    // Checking the registered services in every 20th seconds.
    std::map<pid_t, std::vector<std::unique_ptr<service_checkins_t>>> serviceCheckins;
    int64_t lastServiceCheckin = zclock_mono();
    const uint16_t serviceCheckPeriod = 20000; // In msec.

    while (!terminated){
        void *which = zpoller_wait(poller, REGULAR_MAINTAIN_PERIOD);

        // Check services whether they are still alive.
        // Reregister the too old services (OpenDHT ValueType settings, it is 10 minutes by default)
        if ((zclock_mono()-lastServiceCheckin) > serviceCheckPeriod){
            maintainRenewal(serviceCheckins, dhtNode);
        }

        // Handling messages from the caller (e.g.: $TERM$)
        if (which == pipe) {
            terminated = handlePipeMessage((zsock_t*)which, dhtNode);
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
            terminated = handleRiapsMessages((zsock_t*)which,
                                             clients,
                                             serviceCheckins,
                                             clientSubscriptions,
                                             registeredListeners,
                                             host_address,
                                             mac_address,dhtNode);
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
    zsock_destroy(&dht_router_socket);

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
        if (!kill(it->first,0)==0){
            toBeRemoved.push_back(it->first);
        }
    }

    // Remove killed PIDs
    for (auto it = toBeRemoved.begin(); it!=toBeRemoved.end(); it++){
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



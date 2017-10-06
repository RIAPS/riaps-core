#include <discoveryd/r_riaps_actor.h>
#include <discoveryd/r_discoveryd_commands.h>
#include <discoveryd/r_odht.h>
#include <discoveryd/r_msghandler.h>
#include <framework/rfw_configuration.h>
#include <framework/rfw_network_interfaces.h>

#include <vector>
#include <map>



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

    riaps::DiscoveryMessageHandler msgHandler(dhtNode, pipe);
    msgHandler.Init();
    msgHandler.Run();


    dhtNode.join();
    zclock_sleep(1000);
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



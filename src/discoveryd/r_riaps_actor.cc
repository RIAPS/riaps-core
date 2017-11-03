#include <discoveryd/r_riaps_actor.h>
#include <discoveryd/r_msghandler.h>
#include <framework/rfw_configuration.h>
#include <framework/rfw_network_interfaces.h>

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

    riaps::DiscoveryMessageHandler msgHandler(dhtNode, &pipe);
    msgHandler.Init();
    msgHandler.Run();


    dhtNode.join();
    zclock_sleep(1000);
}










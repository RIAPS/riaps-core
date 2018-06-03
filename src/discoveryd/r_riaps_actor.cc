#include <discoveryd/r_riaps_actor.h>
#include <discoveryd/r_msghandler.h>
#include <framework/rfw_configuration.h>
#include <framework/rfw_network_interfaces.h>

#include <spdlog/spdlog.h>

namespace spd=spdlog;

void
riaps_actor (zsock_t *pipe, void *args)
{
    auto console = spd::stdout_color_mt("rdiscovery");
    console->set_level(spd::level::info);

    std::string mac_address = riaps::framework::Network::GetMacAddressStripped();
    std::string host_address = riaps::framework::Network::GetIPAddress();

    console->info("Starting DHT node.");
    //console->info("Starting DHT backup node.");
    dht::DhtRunner dhtNode;
    //dht::DhtRunner backupNode;

    // Launch a dht node on a new thread, using a
    // generated RSA key pair, and listen on port 4222.

    //dht_node.run(RIAPS_DHT_NODE_PORT, dht::crypto::generateIdentity(), true);
    dhtNode.run(RIAPS_DHT_NODE_PORT, {}, true);
    //backupNode.run(RIAPS_DHT_NODE_PORT-1, {}, true);

    //console->info("Backup Node is connecting to: {}:{}",host_address, RIAPS_DHT_NODE_PORT);
    //backupNode.bootstrap(host_address, std::to_string(RIAPS_DHT_NODE_PORT));

    console->info("DHT is initialized on {}:{}({})", host_address, RIAPS_DHT_NODE_PORT, mac_address);

    //std::cout << "DHT node started." <<std::endl;

    std::srand(std::time(0));

    riaps::DiscoveryMessageHandler msgHandler(dhtNode, &pipe, console);
    msgHandler.init();
    msgHandler.run();


    dhtNode.join();
    zclock_sleep(1000);
}










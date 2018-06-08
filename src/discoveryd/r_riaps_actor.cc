#include <discoveryd/r_riaps_actor.h>
#include <discoveryd/r_msghandler.h>
#include <framework/rfw_configuration.h>
#include <framework/rfw_network_interfaces.h>
#include <discoveryd/r_dhttracker.h>

#include <spdlog/spdlog.h>

namespace spd=spdlog;

void
riaps_actor (zsock_t *pipe, void *args)
{
    auto console = spd::stdout_color_mt("rdiscovery");
    console->set_level(spd::level::info);

    std::string mac_address = riaps::framework::Network::GetMacAddressStripped();
    std::string host_address = riaps::framework::Network::GetIPAddress();

    console->info("Starting DHT node on {}:{} ({})", host_address, RIAPS_DHT_NODE_PORT, mac_address);
    //console->info("Starting DHT backup node.");
    dht::DhtRunner dhtNode;
    //dht::DhtRunner backupNode;

    // Launch a dht node on a new thread, using a
    // generated RSA key pair, and listen on port 4222.

    //dht_node.run(RIAPS_DHT_NODE_PORT, dht::crypto::generateIdentity(), true);

    zsock_signal (pipe, 0);
    try {
        dhtNode.run(RIAPS_DHT_NODE_PORT, {}, true);
        zsock_send(pipe, "1", 1);
        //char* pipeReturn = "1";
        //zstr_send(pipe, pipeReturn);
    } catch (dht::DhtException& e){
        console->error("DHT threw an exception: {}", e.what());
        zsock_send(pipe, "1", 0);
        //char* pipeReturn = "0";
        //zstr_send(pipe, pipeReturn);
        zclock_sleep(500);
        return;
    }



    //backupNode.run(RIAPS_DHT_NODE_PORT-1, {}, true);

    //console->info("Backup Node is connecting to: {}:{}",host_address, RIAPS_DHT_NODE_PORT);
    //backupNode.bootstrap(host_address, std::to_string(RIAPS_DHT_NODE_PORT));

    //console->info("DHT is initialized on {}:{}({})", host_address, RIAPS_DHT_NODE_PORT, mac_address);

    //std::cout << "DHT node started." <<std::endl;

    std::srand(std::time(0));

    riaps::DiscoveryMessageHandler msgHandler(dhtNode, &pipe, console);
    msgHandler.init();
    msgHandler.run();
    dhtNode.join();

    dhtNode.shutdown([console](){
        console->info("DHT stopped");
    });

    zclock_sleep(100);
}










#include <discoveryd/r_riaps_actor.h>
#include <discoveryd/r_msghandler.h>
#include <framework/rfw_configuration.h>
#include <framework/rfw_network_interfaces.h>
#include <discoveryd/r_dhttracker.h>

#include <spdlog_setup/conf.h>

using namespace std;
using namespace riaps::discovery;
namespace spd=spdlog;

namespace riaps::discovery {


    void riaps_actor(zsock_t *pipe, void *args) {
        auto console = spd::get(DISCO_LOGGER_NAME);

        string mac_address = riaps::framework::Network::GetMacAddressStripped();
        string host_address = riaps::framework::Network::GetIPAddress();

        console->info("Starting DHT node on {}:{} ({})", host_address, RIAPS_DHT_NODE_PORT, mac_address);
        dht::DhtRunner dht_node;
        dht::DhtRunner backup_node;

        // Launch a dht node on a new thread, using a
        // generated RSA key pair, and listen on port 4222.
        //dht_node.run(RIAPS_DHT_NODE_PORT, dht::crypto::generateIdentity(), true);

        zsock_signal(pipe, 0);
        try {
            dht_node.run(RIAPS_DHT_NODE_PORT, {}, true);
            backup_node.run(RIAPS_DHT_NODE_PORT - 1, {}, true);
            backup_node.bootstrap(host_address, to_string(RIAPS_DHT_NODE_PORT));
            zsock_send(pipe, "1", 1);
        } catch (dht::DhtException &e) {
            console->error("DHT threw an exception: {}", e.what());
            zsock_send(pipe, "1", 0);
            zclock_sleep(500);
            return;
        }

        srand(time(0));
        DiscoveryMessageHandler handler(dht_node, &pipe, console);
        handler.Init();
        handler.Run();
        dht_node.shutdown([console]() {
            console->info("Public DHT stopped");
        });
        backup_node.shutdown([console]() {
            console->info("Backup DHT stopped");
        });

        zclock_sleep(100);
    }
}










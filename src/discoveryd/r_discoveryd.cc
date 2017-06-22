/*!
 * RIAPS Discovery Service
 *
 * Goal of the service:
 *   - Discover other RIAPS nodes on the local network
 *   - Maintain an ip address cache about the discovered RIAPS nodes
 *   - Control the local Discovery Service client (currently OpenDHT)
 *
 * CZMQ zbeacon  used to discover other riaps nodes in the local network. Each riaps node send a UDP packet periodically,
 *
 * \author Istvan Madari
 * \return
 */



#include <discoveryd/r_riaps_actor.h>
#include <discoveryd/r_discoveryd_commands.h>
#include <utils/r_utils.h>
#include <version/versions.h>

#include <iostream>
#include <string>
#include <map>
#include <vector>

// Frequency of sending UDP packets.
// Starting with higher rate and then switch to lower rate.
#define HIGH_BEACON_FREQ 5000
#define LOW_BEACON_FREQ 10000

// Timeout of port polling for UDP packages
#define UDP_READ_TIMEOUT 500

#define UDP_PACKET_PORT 9999

#define CONTROL_SOCKET "ipc:///tmp/discoverycontrol"

//#define NO_UDP_BEACON


int main()
{
    std::cout << "Starting RIAPS DISCOVERY SERVICE " << RIAPS_DISCOVERY_PRINTABLE_VERSION << std::endl;

    zactor_t *r_actor = zactor_new(riaps_actor, NULL);
    zsock_t * control = zsock_new_router(CONTROL_SOCKET);

#ifndef NO_UDP_BEACON
    // CZMQ zbeacons are used to discover the local network
    // New zbeacon for publishing IP
    zactor_t *speaker = zactor_new (zbeacon, NULL);

    // listen for UDP packages
    zactor_t *listener  = zactor_new (zbeacon, NULL);



    #ifdef _DEBUG_
        zstr_sendx (speaker, "VERBOSE", NULL);
        zstr_sendx (listener, "VERBOSE", NULL);
    #endif

    // configure zbeacon publisher
    zsock_send (speaker, "si", "CONFIGURE", UDP_PACKET_PORT);

    // configure zbeacon listener
    zsock_send (listener, "si", "CONFIGURE", UDP_PACKET_PORT);

    // Check the listener
    char* hostname = zstr_recv (listener);
    if (!*hostname) {
        printf ("No listener hostname, no UDP listening.\n");

        zactor_destroy (&listener);
        free (hostname);
        return -1;
    }
    free (hostname);
    
    // check the publisher
    hostname = zstr_recv (speaker);
    if (!*hostname) {
        printf ("No speaker hostname, no UDP broadcasting\n");
        zactor_destroy (&speaker);
        free (hostname);
        return -1;
    }
    free (hostname);

    // We will broadcast the magic value 0xCAFE
    byte announcement [2] = { 0xCA, 0xFE };

    zsock_send (speaker, "sbi", "PUBLISH", announcement, 2, HIGH_BEACON_FREQ);

    zsock_send (listener, "sb", "SUBSCRIBE", "", 0);
#endif

    // Store the ip addresses and timestamps here
    std::map<std::string, int64_t> ipcache;

    // Node ips from commands, not from UDP packages
    // TODO: Timeout for external nodes
    std::map<std::string, int64_t> externalipcache;

    bool has_joined = false;

    zpoller_t* poller = zpoller_new(control, NULL);

    while (!zsys_interrupted) {
#ifndef NO_UDP_BEACON
        // Wait for at most UDP_READ_TIMEOUT millisecond for UDP package
        zsock_set_rcvtimeo (listener, UDP_READ_TIMEOUT);
#endif
        void *which = zpoller_wait(poller, 1);

        // Command arrived from external scripts
        if (which == control){
            zmsg_t* msg = zmsg_recv(which);
            if (!msg) {
                std::cout << "No msg from external scripts => interrupted" << std::endl;
            } else{
                char* header = zmsg_popstr(msg);
                char* command = zmsg_popstr(msg);
                if (streq(command, CMD_JOIN)){
                    char* newhost = zmsg_popstr(msg);
                    if (newhost){

                        // Check if the node already connected
                        bool is_newitem = externalipcache.find(std::string(newhost)) == externalipcache.end();
                        if (is_newitem){
                            std::cout << "Join to DHT node: " << newhost << std::endl;

                            int64_t time = zclock_mono();
                            externalipcache[newhost] = time;

                            zmsg_t* join_msg = zmsg_new();
                            zmsg_addstr(join_msg, CMD_JOIN);
                            zmsg_addstr(join_msg, newhost);
                            zmsg_send(&join_msg, r_actor);
                        }
                        zstr_free(&newhost);
                    }
                }

                zstr_free(&command);
                zstr_free(&header);
                zmsg_destroy(&msg);
            }

        }

        if (!has_joined && ipcache.size()>1){
            zmsg_t* join_msg = zmsg_new();
            zmsg_addstr(join_msg, CMD_JOIN);

            // Compose the JOIN command with the available ipaddresses
            for (auto it=ipcache.begin(); it!=ipcache.end(); it++){
                zmsg_addstr(join_msg, it->first.c_str());
            }
            
            zmsg_send(&join_msg, r_actor);

            has_joined=true;
        }

        // If the node joined previously, but the clients left the cluster,
        // then try to join again
        else if (has_joined && ipcache.size()==1) {
            has_joined = false;
        }

        // Source of the incoming UDP package
#ifndef NO_UDP_BEACON
        char *ipaddress = zstr_recv (listener);
        if (ipaddress) {
            
            // Check if the item already in the map
            bool is_newitem = ipcache.find(std::string(ipaddress)) == ipcache.end();

            // If the ipaddress in the cache, update the timestamp
            // If the ipaddress is new, add to the cache
            int64_t time = zclock_mono();
            ipcache[ipaddress] = time;

            // Check for outdated ip addresses (no UDP paca)
            bool is_maintained = maintain_cache(ipcache);

            // If a new item arrive or an old one was deleted => print the contents
            if (is_newitem || is_maintained) {
                print_cacheips(ipcache);
            }

            zframe_t *content = zframe_recv (listener);
            assert (zframe_size (content) == 2);
            assert (zframe_data (content) [0] == 0xCA);
            assert (zframe_data (content) [1] == 0xFE);
            zframe_destroy (&content);
            zstr_free (&ipaddress);
            //zstr_sendx (speaker, "SILENCE", NULL);
        } else{
            bool is_maintained = maintain_cache(ipcache);
            if (is_maintained){
                print_cacheips(ipcache);
            }
        }
#endif
    }


    zpoller_destroy(&poller);
    zsock_destroy(&control);
    zactor_destroy(&r_actor);

#ifndef NO_UDP_BEACON

    zactor_destroy(&listener);
    zactor_destroy(&speaker);
#endif

    sleep(2);
    
    return 0;
}

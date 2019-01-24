/*!
 * RIAPS Discovery Service
 *
 * Goal of the service:
 *   - Discover other RIAPS nodes on the local network
 *   - Maintain an ip address cache about the discovered RIAPS nodes
 *   - Control the local Discovery Service client (currently OpenDHT)
 *
 * CZMQ zbeacon  used to discover other riaps nodes in the local network. Each riaps node send a UDP packet periodically.
 *
 * \author Istvan Madari
 */


#include <const/r_const.h>
#include <discoveryd/r_riaps_actor.h>
#include <discoveryd/r_dhttracker.h>
#include <discoveryd/r_validator.h>
#include <framework/rfw_network_interfaces.h>
#include <utils/r_utils.h>
#include <utils/r_timeout.h>

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <experimental/filesystem>

// Frequency of sending UDP packets (msec)
#define BEACON_FREQ 1000

// Timeout of port polling for UDP packages
#define UDP_READ_TIMEOUT 3000

// Port to be used for sending/receiving UDP beacon packages
#define UDP_PACKET_PORT 9999



namespace spd = spdlog;
namespace fs = std::experimental::filesystem;

using namespace std;

shared_ptr<dht::crypto::PrivateKey> getPrivateKey_s(fs::path& key_path ) {
    ifstream privfs(key_path);
//    if (!privfs.good())
//        console->error("No key");

    std::stringstream buffer;
    buffer << privfs.rdbuf();
    vector<uint8_t> blob_key;
    string s = buffer.str();
    transform(s.begin(), s.end(), back_inserter(blob_key),
              [](char c) -> uint8_t {
                  return (uint8_t)c;
              });
    return make_shared<dht::crypto::PrivateKey>(blob_key);
}
//
//shared_ptr<dht::crypto::PrivateKey> getPrivateKey_b(fs::path& key_path ) {
//    ifstream privfs2;
//    privfs2.open(key_path, ios::ate);
//    auto pos = privfs2.tellg();
//    std::vector<uint8_t> buffer2(pos);
//    privfs2.seekg(0, ios::beg);
//    privfs2.read((char*)buffer2.begin().base(), pos);
//    return make_shared<dht::crypto::PrivateKey>(buffer2);
//}
//

//
//dht::Blob sign(const dht::Blob& data, std::shared_ptr<dht::crypto::PrivateKey> private_key) {
//    auto signature = private_key->sign(data);
//    auto converted = reinterpret_cast<char*>(signature.data());
//
//    const int max_dst_size = LZ4_compressBound(signature.size());
//    unique_ptr<char> compressed_signature(new char[max_dst_size]);
//    auto compressed_size = LZ4_compress_limitedOutput(converted, compressed_signature.get(), signature.size(), 255);
//    if (compressed_size<0)
//        cout << "Compression sux";
//    else if (compressed_size == 0)
//        cout << "Empty output in compression";
//    else
//        cout << "Compression is good";
//
//    auto result = ConvertToBlob(compressed_signature.get(), compressed_size);
//    return result;
//}
//
//bool check_signature(const std::string& data, dht::Blob& compressed_signature, std::shared_ptr<dht::crypto::PrivateKey> private_key) {
//    vector<char> decompressed_buffer(512);
//    const int decompressed_size = LZ4_decompress_safe(reinterpret_cast<char*>(compressed_signature.data()), decompressed_buffer.data(), compressed_signature.size(), 512);
//    if (decompressed_size<0)
//        cout << "Decompression sux";
//    else if (decompressed_size == 0)
//        cout << "Empty output in decompression";
//    else
//        cout << "Decompression is good";
//
//    return private_key->getPublicKey().checkSignature(ConvertToBlob(data), ConvertToBlob(decompressed_buffer.data(), decompressed_size));
//}



int main(int argc, char* argv[])
{

    auto console = spd::stdout_color_mt(DISCO_LOGGER_NAME);
    console->set_level(spd::level::debug);
    console->info("Starting RIAPS DISCOVERY SERVICE ");



    // Random generator for beacon interval
    std::random_device rd;
    std::mt19937 gen(rd());

    // UDP packet interval is between 2-5sec
    std::uniform_int_distribution<> dis(2, 5);

    string iface = riaps::framework::Network::GetConfiguredIface();
    string address;
    if (iface != "") {
        zsys_set_interface(iface.c_str());
        address = riaps::framework::Network::GetIPAddress(iface);
    }
    else {
        riaps::framework::Network::GetFirstGlobalIface(iface, address);
        assert(iface!="");
        zsys_set_interface(iface.c_str());
    }




    zactor_t *r_actor = zactor_new(riaps_actor, NULL);
    uint8_t dhtStarted = 0;
    zsock_recv(r_actor, "1", &dhtStarted);

    if (dhtStarted) {
        console->info("DHT thread is initialized, starts beaconing.");
    }else {
        console->error("Unable to initialize DHT thread, exiting.");
        zactor_destroy(&r_actor);
        zclock_sleep(500);
        return -1;
    }

    zsock_t * control = zsock_new_router(CONTROL_SOCKET);

    // zbeacon for sending UDP beacons
    zactor_t *speaker = zactor_new (zbeacon, NULL);

    // listen for UDP packages
    zactor_t *listener  = zactor_new (zbeacon, NULL);

    zsock_t* dhtTracker = zsock_new_dealer(CHAN_IN_DHTTRACKER);

    #ifdef _DEBUG_
        zstr_sendx (speaker, "VERBOSE", NULL);
        zstr_sendx (listener, "VERBOSE", NULL);
    #endif

    // configure zbeacon publisher
    zsock_send (speaker, "si", "CONFIGURE", UDP_PACKET_PORT);

    // configure zbeacon listener
    zsock_send (listener, "si", "CONFIGURE", UDP_PACKET_PORT);

    // Load the private key
    const char *homedir;

    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }

    fs::path key_path(homedir);
    key_path /= KEY_FOLDER;
    key_path /= KEY_FILE;

    auto private_key = getPrivateKey_s(key_path);

    // Create validator
    DiscoveryValidator validator(address, private_key);

    // Check the listener
    char* hostname = zstr_recv (listener);
    if (!*hostname) {
        console->critical("No listener hostname, no UDP listening.\n");

        zactor_destroy (&listener);
        free (hostname);
        return -1;
    }
    free (hostname);
    
    // check the publisher
    hostname = zstr_recv (speaker);
    if (!*hostname) {
        console->critical("No speaker hostname, no UDP listening.\n");
        zactor_destroy (&speaker);
        free (hostname);
        return -1;
    }
    free (hostname);

    // Broadcast the magic value 0xCAFE
    //byte announcement [2] = { 0xCA, 0xFE };

    // PUBLISHING only when we have to. (when no package from the others)
    //zsock_send (speaker, "sbi", "PUBLISH", announcement, 2, HIGH_BEACON_FREQ);

    zsock_send (listener, "sb", "SUBSCRIBE", "", 0);

    // Store the ip addresses and timestamps here
    //std::map<std::string, int64_t> ipcache;
    std::map<std::string, riaps::utils::Timeout<std::chrono::milliseconds>> ipcache;


    // Node ips from commands, not from UDP packages
    // TODO: Timeout for external nodes
    std::map<std::string, int64_t> externalipcache;

    bool has_joined = false;

    zpoller_t* poller = zpoller_new(control, NULL);

    // Generate the time of the next announcement.
    //int nextDiff = dis(gen)*1000;
    //int64_t nextAnnouncement = zclock_mono() + nextDiff;

    //zsock_send (speaker, "sbi", "PUBLISH", announcement, 2, BEACON_FREQ);

    //    auto signed_data    = sign(ConvertToBlob(address), private_key);

    auto udp_payload = ConvertToBlob(validator.validator_address());
    zsock_send (speaker, "sbi", "PUBLISH", udp_payload.data(), udp_payload.size(), BEACON_FREQ);

    zpoller_add(poller, listener);
    while (!zsys_interrupted) {
        // Wait for at most UDP_READ_TIMEOUT millisecond for UDP package
        //zsock_set_rcvtimeo (listener, UDP_READ_TIMEOUT);

        // If no announcement, start sending beacons
//        if (zclock_mono()>nextAnnouncement){
//
//            //LOG(INFO) << "Send UDP beacon";
//
//            zsock_send (speaker, "sbi", "PUBLISH", announcement, 2, BEACON_FREQ);
//
//            // wait for sending out the packet
//            zclock_sleep(100);
//
//            // Stop sending beacons
//            zsock_send(speaker, "s", "SILENCE");
//
//            // Calculate the next announcment
//            int nextDiff = dis(gen)*1000;
//            nextAnnouncement = zclock_mono() + nextDiff;
//        }

        void *which = zpoller_wait(poller, UDP_READ_TIMEOUT);
        validator.ReplyAll();

        // Command arrived from external scripts
        if (which == control){
            zmsg_t* msg = zmsg_recv(which);
            if (!msg) {
                console->error("No msg from external scripts => interrupted");
            } else{
                char* header = zmsg_popstr(msg);
                char* command = zmsg_popstr(msg);
                if (streq(command, CMD_DISCO_JOIN)){
                    char* newhost = zmsg_popstr(msg);
                    if (newhost){

                        // Check if the node already connected
                        bool is_newitem = externalipcache.find(std::string(newhost)) == externalipcache.end();
                        if (is_newitem){
                            //console->info("Join to DHT node: {}", newhost);
                            //std::cout <<  << newhost << std::endl;

                            int64_t time = zclock_mono();
                            externalipcache[newhost] = time;

                            zmsg_t* join_msg = zmsg_new();
                            zmsg_addstr(join_msg, CMD_DISCO_JOIN);
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

        } else if (which == listener) {
            char *ipaddress = zstr_recv (listener);

            // If UDP package was received
            if (ipaddress) {
                auto validator_address   = zstr_recv(listener);
                if (validator_address && ipaddress!=address) {
                    if (validator.IsValid(validator_address)) {

                        // Pass the ip addres to the dht tracker to check its stability
                        // DHT must be stable for at least 3 seconds before the registration happens
                        zsock_send(dhtTracker, "ss", CMD_BEACON_IP, ipaddress);

                        if (strcmp(ipaddress, address.c_str()) != 0) {
                            // Check if the item already in the map
                            bool is_newitem = ipcache.find(std::string(ipaddress)) == ipcache.end();

                            // If the ipaddress in the cache, update the timestamp
                            // If the ipaddress is new, add to the cache
                            //int64_t time = zclock_mono();
                            ipcache[ipaddress] = riaps::utils::Timeout<std::chrono::milliseconds>(
                                    std::chrono::milliseconds(IPCACHE_TIMEOUT));

                            // Check for outdated ip addresses (no UDP paca)
                            bool is_maintained = maintain_cache(ipcache);

                            // If a new item arrive or an old one was deleted => print the contents
                            if (is_newitem || is_maintained) {
                                print_cacheips(ipcache, address);
                            }

                            if (is_newitem) {
                                zmsg_t *join_msg = zmsg_new();
                                zmsg_addstr(join_msg, CMD_DISCO_JOIN);
                                zmsg_addstr(join_msg, ipaddress);
                                zmsg_send(&join_msg, r_actor);
                            }
                        }
                    }
                    zstr_free (&validator_address);
                }
                zstr_free (&ipaddress);
            }
        } // Source of the incoming UDP package
        else{
            bool is_maintained = maintain_cache(ipcache);
            if (is_maintained){
                print_cacheips(ipcache, address);
            }
        }

//        if (!has_joined && ipcache.size()>1){
//            zmsg_t* join_msg = zmsg_new();
//            zmsg_addstr(join_msg, CMD_JOIN);
//
//            // Compose the JOIN command with the available ipaddresses
//            for (auto it=ipcache.begin(); it!=ipcache.end(); it++){
//                zmsg_addstr(join_msg, it->first.c_str());
//            }
//
//            zmsg_send(&join_msg, r_actor);
//
//            has_joined=true;
//        }
//
//        // If the node joined previously, but the clients left the cluster,
//        // then try to join again
//        else if (has_joined && ipcache.size()==1) {
//            has_joined = false;
//        }


    }
    zsock_send(speaker, "s", "SILENCE");
    zclock_sleep(500);
    zpoller_destroy(&poller);
    zsock_destroy(&dhtTracker);
    zsock_destroy(&control);
    zactor_destroy(&r_actor);
    zactor_destroy(&listener);
    zactor_destroy(&speaker);

    // Wait for the threads
    zclock_sleep(1000);
    
    return 0;
}

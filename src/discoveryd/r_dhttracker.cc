//
// Created by istvan on 6/5/18.
//

#include <discoveryd/r_dhttracker.h>

#define CHAN_OUT_DHTTRACKER "inproc://out_dhttracker"
#define CHAN_IN_DHTTRACKER  "inproc://in_dhttracker"

namespace spd = spdlog;

using namespace std;

void dht_tracker (zsock_t *pipe, void *args) {
    //std::map<std::string, riaps::utils::Timeout<std::milli>> beacon_list_;
    //std::map<std::string, riaps::utils::Timeout<std::milli>> odht_list_;

    auto dht = static_cast<dht::DhtRunner*>(args);
    std::map<std::string, bool> node_list;

    zsock_signal (pipe, 0);

    zsock_t* out_socket = zsock_new_dealer(CHAN_OUT_DHTTRACKER);
    zsock_t* in_socket  = zsock_new_router(CHAN_IN_DHTTRACKER);
    zpoller_t* poller = zpoller_new(pipe, in_socket, nullptr);

    auto logger = spd::stdout_color_mt("dht_tracker");

    bool terminated = false;

    while (!terminated) {
        auto which = zpoller_wait(poller, 1000);

        if (which == pipe) {
            zmsg_t* msg = zmsg_recv(which);
            char *command = zmsg_popstr(msg);

            if (streq(command, "$TERM")) {
                //std::cout << std::endl << "$TERMINATE arrived, discovery service is stopping..." << std::endl;
                logger->info("$TERMINATE arrived, discovery service is stopping.");
                terminated = true;
            }
            zstr_free(&command);
            zmsg_destroy(&msg);
        } else if (which == in_socket) {
            char* command;
            char* param;
            std::string sparam(param);

            zsock_recv(which, "ss", &command, &param);
            if (streq(command, CMD_BEACON_IP)) {
                // New bacon arrived
                if (node_list.find(sparam) == node_list.end()) {
                    node_list[sparam] = false;
                }

            } else if (streq(command, CMD_DHT_IP)) {
                if (node_list.find(sparam) != node_list.end()) {
                    node_list[sparam] = true;
                }
            }

            zstr_free(&command);
            zstr_free(&param);
        }
        // Check if stable
        else {
            // All true
//            auto isStable = std::all_of(node_list.begin(), node_list.end(), [](pair<string, bool>& item) {
//                return item.second;
//            });

//            if ({
//                return item.second;
//            })) {
//                zsock_send(out_socket, "s1", CMD_DHT_STABLE, 1);
//            }else {
//                zsock_send(out_socket, "s1", CMD_DHT_STABLE, 0);
//
//                for(auto& item : node_list) {
//                    if (!item.second) {
//                        dht::InfoHash keyhash(item.first);
//                        auto f = dht->get(keyhash);
//                        f.wait();
//                        auto values = f.get();
//                        if (values.size() != 0) {
//                            node_list[item.first] = true;
//                        }
//                    }
//                }
//            };
        }
    }
}

//
// Created by istvan on 6/5/18.
//

#include <discoveryd/r_dhttracker.h>


#define LOGGER_NAME "dht_tracker"

namespace spd = spdlog;

using namespace std;



void dht_tracker (zsock_t *pipe, void *args) {
    auto dht = static_cast<dht::DhtRunner*>(args);
    std::map<std::string, bool> node_list;

    zsock_signal (pipe, 0);

    zsock_t* in_socket  = zsock_new_router(CHAN_IN_DHTTRACKER);
    zpoller_t* poller = zpoller_new(pipe, in_socket, nullptr);

    auto logger = spd::stdout_color_mt(LOGGER_NAME);
    logger->set_level(spd::level::info);

    bool terminated = false;
    bool isStable = false;
    riaps::utils::Timeout<std::milli> lastStable(duration<int, std::milli>(3000)); //3sec

    logger->debug("starts");
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
            char* frm;

            zsock_recv(which, "sss", &frm, &command, &param);
            std::string sparam(param);

            if (streq(command, CMD_BEACON_IP)) {
                // New bacon arrived
                if (node_list.find(sparam) == node_list.end()) {
                    logger->debug("{}({})", CMD_BEACON_IP, sparam);
                    node_list[sparam] = false;
                    dht::InfoHash keyhash(sparam);
                    std::vector<uint8_t> opendht_data(sparam.begin(), sparam.end());
                    dht->put(keyhash, sparam);
                    isStable = check_state(node_list, *dht);
                }

            } else if (streq(command, CMD_DHT_IP)) {
                logger->debug("{}({})", CMD_DHT_IP, sparam);
                if (node_list.find(sparam) != node_list.end()) {
                    node_list[sparam] = true;
                    isStable = check_state(node_list, *dht);
                }
            } else if (streq(command, CMD_QUERY_STABLE)) {
                logger->debug("{}->{}", CMD_QUERY_STABLE, isStable);
                zsock_send(in_socket, "s1", frm, isStable);
            }

            zstr_free(&frm);
            zstr_free(&command);
            zstr_free(&param);
        }
    }
    logger->info("stopped");
    zpoller_destroy(&poller);
    zsock_destroy(&in_socket);
}

bool check_state(std::map<std::string, bool>& node_list, dht::DhtRunner& dht) {
    auto result = false;
    auto logger = spd::get(LOGGER_NAME);
    auto stableCount = count_if(node_list.begin(), node_list.end(), [](const pair<string, bool>& item){
        return item.second;
    });

    auto unstableCount = count_if(node_list.begin(), node_list.end(), [](const pair<string, bool>& item){
        return !item.second;
    });

    logger->debug("Stable/unstable: {}/{}", stableCount, unstableCount);

    if (unstableCount == 0 && stableCount>0) {
        result = true;
        logger->debug("Stable");
    } else {
        //zsock_send(out_socket, "s1", CMD_DHT_STABLE, 0);
        vector<tuple<string, shared_future<vector<shared_ptr<dht::Value>>>>> waitList;
        //logger->debug("Not stable.");

        for(auto& item : node_list) {
            if (!item.second) {
                logger->debug("Not stable: {}", item.first);
                dht::InfoHash keyhash(item.first);
                std::future<std::vector<std::shared_ptr<dht::Value>>> task = dht.get(keyhash);
                auto sharedTask = std::shared_future<std::vector<std::shared_ptr<dht::Value>>>(std::move(task));
                waitList.push_back(make_tuple(item.first, sharedTask));
            }
            else {
                logger->debug("Stable: {}", item.first);
            }
        }

        for (auto& taskItem : waitList) {
            auto f = get<1>(taskItem);
            f.wait();
            auto results = f.get();
            if (results.size()>0) {
                auto ipaddress = get<0>(taskItem);
                std::async(std::launch::async, [ipaddress](){
                    zsock_t* puship = zsock_new_dealer(CHAN_IN_DHTTRACKER);
                    zsock_send(puship, "ss", CMD_DHT_IP, ipaddress.c_str());
                    zclock_sleep(100);
                    zsock_destroy(&puship);
                });

                //node_list[get<0>(taskItem)] = true;
                //logger->debug("Node {} is stable from now", get<0>(taskItem));
            }
        }
    }

    return result;
}

//
// Created by istvan on 6/5/18.
//

#include <discoveryd/r_dhttracker.h>
#include <algorithm>
#include <framework/rfw_network_interfaces.h>
#include <spdlog/sinks/stdout_color_sinks.h>


constexpr auto LOGGER_NAME = "dht_tracker";

namespace spd = spdlog;

using namespace std;
using namespace riaps::utils;

/**
 * ZActor thread for checking the DHT stability
 * @param pipe PAIR socket for messaging with the parent class.
 * @param args Pointer to the DHT instance
 */
void dht_tracker (zsock_t *pipe, void *args) {
    auto dht = static_cast<dht::DhtRunner*>(args);
    unordered_map<string, tuple<bool, Timeout<std::chrono::minutes>>> node_list;

    zsock_signal (pipe, 0);

    zsock_t* in_socket  = zsock_new_router(CHAN_IN_DHTTRACKER);
    zpoller_t* poller = zpoller_new(pipe, in_socket, nullptr);

    auto logger = spd::stdout_color_mt(LOGGER_NAME);
    logger->set_level(spd::level::info);

    bool terminated = false;
    bool isStable   = false;

    logger->debug("starts");

    auto publish_ip_to = Timeout<std::chrono::minutes>(10);
    auto republish = [dht, &publish_ip_to](){
        string self_ip = riaps::framework::Network::GetIPAddress();
        dht::InfoHash keyhash(self_ip);
        dht->put(keyhash, self_ip);
        publish_ip_to.Reset();
    };
    republish();
    auto last_refresh = Timeout<chrono::seconds>(30);
    while (!terminated) {
        auto which = zpoller_wait(poller, 2000);

        if (which == pipe) {
            zmsg_t* msg = zmsg_recv(which);
            char *command = zmsg_popstr(msg);

            if (streq(command, "$TERM")) {
                logger->debug("stops");
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
                    node_list[sparam] = make_tuple(false, Timeout<std::chrono::minutes>(10));
                    isStable = check_state(node_list, *dht);
                }
            } else if (streq(command, CMD_DHT_IP)) {
                logger->debug("{}({})", CMD_DHT_IP, sparam);
                if (node_list.find(sparam) != node_list.end() && !get<0>(node_list[sparam])) {
                    get<0>(node_list[sparam]) = true;
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
        if (last_refresh.IsTimeout()) {
            logger->debug("30 sec refresh");
            last_refresh.Reset();

            // Remove timed out items
            for(auto it = node_list.begin(); it != node_list.end();)
            {
                if(get<1>(it->second).IsTimeout()) {
                    logger->debug("Timed out: {}, remove from cache", it->first);
                    it = node_list.erase(it);
                }
                else ++it;
            }

            // Republish self_ip in every 10 minutes
            if (publish_ip_to.IsTimeout()) {
                republish();
                logger->debug("Republishing self");
            }

            // If still not stable recheck
            if (!isStable)
                isStable = check_state(node_list, *dht);
        }
    }
    logger->debug("stopped");
    zpoller_destroy(&poller);
    zsock_destroy(&in_socket);
}

bool check_state(std::unordered_map<std::string, std::tuple<bool, riaps::utils::Timeout<std::chrono::minutes>>>& node_list, dht::DhtRunner& dht) {
    auto result = false;
    auto logger = spd::get(LOGGER_NAME);
    auto stableCount = count_if(node_list.begin(), node_list.end(), [](const pair<string, tuple<bool, Timeout<std::chrono::minutes>>>& item){
        return get<0>(item.second);
    });

    auto unstableCount = count_if(node_list.begin(), node_list.end(), [](const pair<string, tuple<bool, Timeout<std::chrono::minutes>>>& item){
        return !get<0>(item.second);
    });

    logger->debug("Stable/unstable: {}/{}", stableCount, unstableCount);

    if (unstableCount == 0 && stableCount>0) {
        result = true;
        logger->debug("Stable");
    } else {
        vector<tuple<string, shared_future<vector<shared_ptr<dht::Value>>>>> waitList;

        // start DHT.get() asynchonosly, save the std::future for retrieving the results later.
        for(auto& item : node_list) {
            if (!get<0>(item.second)) {
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
            }
        }
    }

    return result;
}

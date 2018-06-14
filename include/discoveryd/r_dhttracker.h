//
// Created by istvan on 6/5/18.
//

#ifndef RIAPS_CORE_R_DHTTRACKER_H
#define RIAPS_CORE_R_DHTTRACKER_H

#define CMD_BEACON_IP  "CMD_BEACON_IP"
#define CMD_DHT_IP     "CMD_DHT_IP"
#define CMD_QUERY_STABLE "CMD_QUERY_STABLE"

#define CHAN_IN_DHTTRACKER  "inproc://in_dhttracker"


#include <utils/r_timeout.h>
#include <opendht.h>
#include <map>
#include <unordered_map>
#include <czmq.h>
#include <spdlog/spdlog.h>



void dht_tracker (zsock_t *pipe, void *args);

bool check_state(std::unordered_map<std::string, std::tuple<bool, riaps::utils::Timeout<std::chrono::minutes>>>& node_list, dht::DhtRunner& dht);


#endif //RIAPS_CORE_R_DHTTRACKER_H

//
// Created by istvan on 6/5/18.
//

#ifndef RIAPS_CORE_R_DHTTRACKER_H
#define RIAPS_CORE_R_DHTTRACKER_H

#define CMD_BEACON_IP  "CMD_BEACON_IP"
#define CMD_DHT_IP     "CMD_DHT_IP"
#define CMD_DHT_STABLE "CMD_DHT_STABLE"


#include <utils/r_timeout.h>
#include <opendht.h>
#include <map>
#include <czmq.h>
#include <spdlog/spdlog.h>



void dht_tracker (zsock_t *pipe, void *args);


#endif //RIAPS_CORE_R_DHTTRACKER_H

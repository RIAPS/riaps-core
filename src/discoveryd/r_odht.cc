//
// Created by istvan on 11/22/16.
//

#include "discoveryd/r_odht.h"

void dht_jointocluster(std::string& destination_host, int port, dht::DhtRunner& dhtrunner){
    dhtrunner.bootstrap(destination_host, std::to_string(port));
}
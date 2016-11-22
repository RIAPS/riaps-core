//
// Created by istvan on 11/22/16.
//

#ifndef RIAPS_FW_R_ODHT_H
#define RIAPS_FW_R_ODHT_H

#include <string>
#include <opendht.h>


#define RIAPS_DHT_NODE_PORT 4222

void dht_jointocluster(std::string&, int port, dht::DhtRunner&);

#endif //RIAPS_FW_R_ODHT_H

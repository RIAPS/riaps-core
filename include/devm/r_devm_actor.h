//
// Created by istvan on 5/4/17.
//

#ifndef RIAPS_CORE_R_DEVM_ACTOR_H
#define RIAPS_CORE_R_DEVM_ACTOR_H

#include <czmq.h>

// Name of endpoint for actor-devm communication
#define DEVM_ENDPOINT "ipc:///tmp/riaps-devm"

//Timeout for actor-devm communication (-1: wait forever)
#define DEVM_ENDPOINTRECVTIMEOUT 1000
#define DEVM_ENDPOINTSENDTIMEOUT 3000

void devm_zactor (zsock_t *pipe, void *args);

#endif //RIAPS_CORE_R_DEVM_ACTOR_H

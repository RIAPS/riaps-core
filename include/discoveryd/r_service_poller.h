//
// Created by parallels on 9/20/16.
//

#ifndef RIAPS_FW_R_SERVICE_POLLER_H
#define RIAPS_FW_R_SERVICE_POLLER_H


#include <czmq.h>
#include <string>
#include <iostream>

#include "r_consul.h"

void service_poller_actor (zsock_t *pipe, void *args);

#endif //RIAPS_FW_R_SERVICE_POLLER_H

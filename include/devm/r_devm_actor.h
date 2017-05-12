//
// Created by istvan on 5/4/17.
//

#ifndef RIAPS_CORE_R_DEVM_ACTOR_H
#define RIAPS_CORE_R_DEVM_ACTOR_H

#include <const/r_endpointconfigs.h>
#include <czmq.h>



void devm_zactor (zsock_t *pipe, void *args);

#endif //RIAPS_CORE_R_DEVM_ACTOR_H

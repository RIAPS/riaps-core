#ifndef R_RIAPS_ACTOR
#define R_RIAPS_ACTOR

#include <czmq.h>
#include <string>
#include <iostream>
#include <vector>

#include "componentmodel/r_discoverdapi.h"
#include "r_consul.h"
#include "utils/r_message.h"
#include "componentmodel/r_riaps_actor_commands.h"


void riaps_actor (zsock_t *pipe, void *args);



#endif
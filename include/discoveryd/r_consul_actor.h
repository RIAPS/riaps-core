#ifndef R_CONSUL_ACTOR
#define R_CONSUL_ACTOR

#include <czmq.h>
#include <string>
#include <iostream>

#include "r_consul.h"

void consul_actor (zsock_t *pipe, void *args);


#endif
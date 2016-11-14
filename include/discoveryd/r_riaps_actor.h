#ifndef R_RIAPS_ACTOR
#define R_RIAPS_ACTOR

#include <capnp/message.h>
#include <capnp/serialize.h>

#include <czmq.h>
#include <string>
#include <iostream>
#include <vector>



#include "componentmodel/r_discoverdapi.h"
#include "r_consul.h"
#include "utils/r_message.h"
#include "componentmodel/r_riaps_actor_commands.h"

struct _actor_details {
    _actor_details(){
        socket=NULL;
    }

    zsock_t* socket;
    int port;

    ~_actor_details(){
        if (socket!=NULL) {
            zsock_destroy(&socket);
        }
    }
};

typedef struct _actor_details actor_details;

void riaps_actor (zsock_t *pipe, void *args);



#endif
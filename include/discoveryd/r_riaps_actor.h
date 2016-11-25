#ifndef R_RIAPS_ACTOR
#define R_RIAPS_ACTOR


#include "r_odht.h"
#include "discoveryd/r_service_poller.h"
#include "discoveryd/r_riaps_cmd_handler.h"
#include "discoveryd/r_odht.h"
#include "componentmodel/r_discoverdapi.h"

#include <czmq.h>



/*
#include "componentmodel/r_discoverdapi.h"
#include "componentmodel/r_riaps_actor_commands.h"
#include "discoveryd/r_riaps_cmd_handler.h"
#include "discoveryd/r_service_poller.h"
#include "utils/r_message.h"
#include "utils/r_utils.h"

#include <capnp/message.h>
#include <capnp/serialize.h>



#include <string>
#include <iostream>
#include <vector>
*/


//#include "utils/r_message.h"


//#include "loggerd/r_loggerd.h"
//#include <unistd.h>
//#include <capnp/common.h>


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


struct _client_details{
    std::string app_name;
    std::string actor_name;
    std::string actor_host;
    std::string instance_name;
    std::string portname;

    bool operator==(const struct _client_details& rhs);
};

typedef struct _client_details client_details;

void riaps_actor (zsock_t *pipe, void *args);



#endif
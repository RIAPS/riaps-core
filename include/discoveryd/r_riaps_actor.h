#ifndef R_RIAPS_ACTOR
#define R_RIAPS_ACTOR



#include "discoveryd/r_service_poller.h"
#include "discoveryd/r_riaps_cmd_handler.h"
#include "discoveryd/r_odht.h"
#include "componentmodel/r_discoverdapi.h"

#include <czmq.h>





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

struct _client_details{
    std::string app_name;
    std::string actor_name;
    std::string actor_host;
    std::string instance_name;
    std::string portname;
    bool        isLocal;

    bool operator==(const struct _client_details& rhs);
};



void riaps_actor (zsock_t *pipe, void *args);



#endif
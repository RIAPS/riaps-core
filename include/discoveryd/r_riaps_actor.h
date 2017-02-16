#ifndef R_RIAPS_ACTOR
#define R_RIAPS_ACTOR



#include "discoveryd/r_riaps_cmd_handler.h"
#include "discoveryd/r_odht.h"
#include "componentmodel/r_discoverdapi.h"

#include <czmq.h>


struct _client_details_t;
struct _actor_details_t;
struct _service_checkins_t;

typedef struct _client_details_t client_details_t;
typedef struct _actor_details_t actor_details_t;
typedef struct _service_checkins_t service_checkins_t;


struct _actor_details_t {
    _actor_details_t(){
        socket=NULL;
    }

    // PAIR socket for sending messages to the actor
    // New socket is created for each actor.
    zsock_t* socket;

    // The port of the socket
    int port;

    // The actor's PID, so we can detect if the porcess died.
    int pid;

    ~_actor_details_t(){
        if (socket!=NULL) {
            zsock_destroy(&socket);
        }
    }
};

struct _client_details_t{
    std::string app_name;
    std::string actor_name;
    std::string actor_host;
    std::string instance_name;
    std::string portname;
    bool        isLocal;


    _client_details_t(){
        isLocal = false;
    }

    _client_details_t(const struct _client_details_t& other){
        app_name = other.app_name;
        actor_name = other.actor_name;
        actor_host = other.actor_host;
        instance_name = other.instance_name;
        portname = other.portname;
        isLocal = other.isLocal;
    }

    //bool operator==(const struct _client_details& rhs);
};

struct _service_checkins_t {
    std::string key;
    std::string value;
    pid_t pid;
    //uint64_t lastCheckinTime;
    uint64_t createdTime;
    uint64_t timeout; // in millisec

    _service_checkins_t(){
        // 10 mins
        timeout = 10*60*1000;
    }
};


void riaps_actor (zsock_t *pipe, void *args);
int deregisterActor(const std::string& appName,
                    const std::string& actorName,
                    const std::string& macAddress,
                    const std::string& hostAddress,
                    std::map<std::string, std::unique_ptr<actor_details_t>>& clients);

void maintainRenewal(std::map<pid_t, std::vector<std::unique_ptr<service_checkins_t>>>& serviceCheckins, dht::DhtRunner& dhtNode);



#endif
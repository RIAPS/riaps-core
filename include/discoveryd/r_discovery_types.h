#ifndef RIAPS_CORE_R_DISCOVERY_TYPES_H
#define RIAPS_CORE_R_DISCOVERY_TYPES_H

#include <czmq.h>

#include <string>


struct ActorDetails {
    ActorDetails(){
        socket=nullptr;
    }

    // PAIR socket for sending messages to the actor
    // New socket is created for each actor.
    zsock_t* socket;

    // The port of the socket
    int port;

    // The actor's PID, so we can detect if the porcess died.
    int pid;

    // Application name
    std::string appName;

    ~ActorDetails(){
        if (socket!=nullptr) {
            zsock_destroy(&socket);
            socket=nullptr;
        }
    }
};

struct ClientDetails{
    std::string app_name;
    std::string actor_name;
    std::string actor_host;
    std::string instance_name;
    std::string portname;
    bool        isLocal;


    ClientDetails(){
        isLocal = false;
    }

    ClientDetails(const struct ClientDetails& other){
        app_name = other.app_name;
        actor_name = other.actor_name;
        actor_host = other.actor_host;
        instance_name = other.instance_name;
        portname = other.portname;
        isLocal = other.isLocal;
    }

    //bool operator==(const struct _client_details& rhs);
};

struct ServiceCheckins {
    std::string key;
    std::string value;
    pid_t pid;
    //uint64_t lastCheckinTime;
    uint64_t createdTime;
    uint64_t timeout; // in millisec

    ServiceCheckins(){
        // 10 mins
        timeout = 10*60*1000;
    }
};

#endif //RIAPS_CORE_R_DISCOVERY_TYPES_H

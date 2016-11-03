//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_R_ACTOR_H
#define RIAPS_R_ACTOR_H

#include <czmq.h>
#include <iostream>
#include <string>
#include <dlfcn.h>


#include <componentmodel/r_discoverdapi.h>
#include <componentmodel/r_componentbase.h>

#include <fstream>
#include <json.h>

namespace riaps {
    class Actor {
    public:
        Actor(std::string actorid);
        virtual void start(std::string configfile="config.json");

        virtual ~Actor();

    protected:
        zpoller_t*                 _poller;

        // Channel for incomming controll messages (e.g.: restart component)
        zsock_t*                   _actor_zsock;

        int                        _actor_port;
        std::string                _actor_endpoint;
        std::string                _actor_id;
        std::vector<ComponentBase> _components;
    };
}

#endif //RIAPS_R_ACTOR_H

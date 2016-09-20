//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_R_ACTOR_H
#define RIAPS_R_ACTOR_H

#include <czmq.h>
#include <iostream>
#include <string>

#include <componentmodel/r_discoverdapi.h>
#include <componentmodel/r_componentbase.h>

namespace riaps {
    class Actor {
    public:
        Actor(std::string actorid);
        void start();

        ~Actor();

    protected:
        zpoller_t*                 _poller;

        // Channel for incomming controll messages (e.g.: restart component)
        zsock_t*                   _actor_zsock;

        // Proxy for actor's components (INPROC)
        //zsock_t*                   _actor_comp_pull;
        //zsock_t*                   _actor_comp_push;
        zactor_t*                  _actor_comp_proxy;

        int                        _actor_port;
        std::string                _actor_endpoint;
        std::string                _actor_id;
        std::vector<ComponentBase> _components;
    };
}

#endif //RIAPS_R_ACTOR_H

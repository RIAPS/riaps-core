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
        Actor();
        void start();

        ~Actor();

    protected:
        zpoller_t*                 poller;
        zsock_t*                   actor_zsock;
        int                        actor_port;
        std::string                actor_endpoint;
        std::vector<ComponentBase> components;
    };
}

#endif //RIAPS_R_ACTOR_H

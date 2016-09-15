//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_FW_ACTOR_PUB_H
#define RIAPS_FW_ACTOR_PUB_H

#include <czmq.h>

#include "componentmodel/r_actor.h"
#include "componentmodel/r_componentbase.h"
#include "component_pub.h"

using riaps::ComponentBase;

class actor_pub : public riaps::Actor {

public:

    actor_pub(){

    }





    void start(){


        component_conf cconf;

        publisher_conf pport;
        pport.servicename = "Publisher1";
        pport.network_iface   = "eth0";
        pport.port            = 0;            // Auto binding

        cconf.component_name = "ComponentPub";
        cconf.publishers_config.push_back(pport);

        component_pub c(cconf);

        while (!zsys_interrupted) {
            void* which = zpoller_wait(poller, 2000);

        }
    }


};

#endif //RIAPS_FW_ACTOR_PUB_H

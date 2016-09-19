//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_FW_ACTOR_SUB_H
#define RIAPS_FW_ACTOR_SUB_H

#include <czmq.h>

#include "componentmodel/r_actor.h"
#include "componentmodel/r_componentbase.h"
#include "component_sub.h"

using riaps::ComponentBase;

class actor_sub : public riaps::Actor {

public:

    actor_sub(){

    }

    void start(){
        component_conf cconf;

        subscriber_conf sport;
        sport.servicename        = "Subscriber1";
        sport.remoteservice_name = "Publisher1";

        cconf.component_name = "ComponentS";
        cconf.subscribers_config.push_back(sport);

        component_sub c(cconf);

        while (!zsys_interrupted) {
            void* which = zpoller_wait(poller, 2000);
            //std::cout << "Poll poll more... Current publishers:" << std::endl;

            //for (auto publisher : c.GetPublisherPorts()){
            //    std::cout << "  - " << publisher->GetEndpoint() <<std::endl;
            //}

        }
    }




};


#endif //RIAPS_FW_ACTOR_SUB_H

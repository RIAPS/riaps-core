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
        actor_zsock = zsock_new_rep("tcp://*:!");
        assert(actor_zsock);
        poller = zpoller_new(actor_zsock);
        assert(poller);
    }

    void init(){
        //Bind to random port from C000 to FFFF
        //actor_port = zsock_bind(actor_zsock, "tcp://*:!");
        //const char* endpoint = zsock_endpoint(actor_zsock);
        //actor_endpoint = std::string(endpoint);
        //std::cout << "Endpoint: " << endpoint << std::endl;
        //std::cout << actor_port <<std::endl;

        register_actor("ActorSub");
    }

    void initComponents(){

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

    ~actor_sub(){
        deregister_actor("ActorSub");
        zpoller_destroy(&poller);
        zsock_destroy(&actor_zsock);
    }

protected:
    zpoller_t*                 poller;
    zsock_t*                   actor_zsock;
    int                        actor_port;
    std::string                actor_endpoint;
    std::vector<ComponentBase> components;
};


#endif //RIAPS_FW_ACTOR_SUB_H

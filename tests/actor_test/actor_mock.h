//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_FW_ACTOR_MOCK_H
#define RIAPS_FW_ACTOR_MOCK_H

#include <czmq.h>

#include "componentmodel/r_actor.h"
#include "componentmodel/r_componentbase.h"

#include "component_mock.h"

using riaps::ComponentBase;

class actor_mock : public riaps::Actor {

public:

    actor_mock(){
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

        register_actor("Actor1");
    }

    void initComponents(){

    }

    void start(){


        component_conf cconf;

        publisher_conf pport;
        pport.servicename = "Publisher1";
        pport.network_iface   = "eth1";
        pport.port            = 0;            // Auto binding

        subscriber_conf sport;
        sport.servicename        = "Subscriber1";
        sport.remoteservice_name = pport.servicename;

        cconf.component_name = "ComponentA";
        cconf.publishers_config.push_back(pport);
        cconf.subscribers_config.push_back(sport);

        component_mock c(cconf);

        while (!zsys_interrupted) {
            void* which = zpoller_wait(poller, 2000);
            std::cout << "Poll poll more... Current publishers:" << std::endl;

            for (auto publisher : c.GetPublisherPorts()){
                std::cout << "  - " << publisher->GetEndpoint() <<std::endl;
            }

        }
    }

    ~actor_mock(){
        deregister_actor("Actor1");
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


#endif //RIAPS_FW_ACTOR_MOCK_H

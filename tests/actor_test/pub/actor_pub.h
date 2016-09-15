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

        int i =0;

        while (!zsys_interrupted) {
            void* which = zpoller_wait(poller, 2000);

            if (which == actor_zsock) {
                std::cout << "Something arrived on pipe";

                zmsg_t *msg = zmsg_recv(which);
                if (!msg) {
                    std::cout << "No msg => interrupted" << std::endl;
                    break;
                }

                char *command = zmsg_popstr(msg);
                std::cout << "Erkezett: " << command <<std::endl;
                free(command);
            }
            else {
                std::cout << "Send message: " << i <<std::endl;

                zmsg_t* msg = zmsg_new();
                zmsg_addstr(msg, std::to_string(i++).c_str());

                c.GetPublisherPorts()[0]->PublishMessage(&msg);
            }
        }
    }


};


#endif //RIAPS_FW_ACTOR_PUB_H

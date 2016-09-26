//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_FW_ACTOR_PUB_H
#define RIAPS_FW_ACTOR_PUB_H

#include <czmq.h>

#include "componentmodel/r_actor.h"
#include "componentmodel/r_componentbase.h"
#include "component_pub.h"
#include "../messagetypes/messages.h"

using riaps::ComponentBase;

class actor_pub : public riaps::Actor {

public:

    actor_pub(std::string actorid) : Actor(actorid){

    }

    void start(){
        component_conf cconf;

        publisher_conf pport;
        pport.servicename = "Publisher1";
        pport.network_iface   = "s1";
        pport.port            = 0;            // Auto binding

        cconf.component_name = "ComponentPub";
        cconf.publishers_config.push_back(pport);

        component_pub c(cconf);

        int i =0;

        while (!zsys_interrupted) {
            void* which = zpoller_wait(_poller, 2000);

            if (which == _actor_zsock) {
                /*std::cout << "Something arrived on pipe";

                zmsg_t *msg = zmsg_recv(which);
                if (!msg) {
                    std::cout << "No msg => interrupted" << std::endl;
                    break;
                }

                char *command = zmsg_popstr(msg);
                std::cout << "Erkezett: " << command <<std::endl;
                free(command);*/
            }
            else {
                //std::cout << "Send message: " << i <<std::endl;

                //zmsg_t* msg = zmsg_new();
                //zmsg_addstr(msg, std::to_string(i++).c_str());
                std::vector<std::string> params;

                std::string istrmsg = "[" + std::to_string(i++) + "]. Message";
                params.push_back(istrmsg);

                zmsg_t* msg = create_message(MSG_ACT_PING, params);

                c.GetPublisherPorts()[0]->PublishMessage(&msg);
            }
        }
    }


};


#endif //RIAPS_FW_ACTOR_PUB_H

//
// Created by parallels on 9/7/16.
//

#ifndef RIAPS_FW_COMPONENT_SUB_H
#define RIAPS_FW_COMPONENT_SUB_H

#include "componentmodel/r_componentbase.h"
#include <string>
#include <czmq.h>



class component_sub : public riaps::ComponentBase {

public:

    component_sub(component_conf& config) : ComponentBase(config){

    };

    virtual void OnMessageArrived(std::string messagetype, zmsg_t* msg_body) {
        std::cout << messagetype << " arrived " << std::endl;

        bool has_more_msg = true;

        while (has_more_msg) {
            char* msgpart = zmsg_popstr(msg_body);
            if (msgpart){
                std::cout << " part: " << msgpart <<std::endl;
                free(msgpart);
            }
            else{
                has_more_msg = false;
            }
        }
    }

    virtual void OnTimerFired(std::string timerid) {

    }

    void init(){

    };

    ~component_sub(){
    };

};

#endif //RIAPS_FW_COMPONENT_SUB_H

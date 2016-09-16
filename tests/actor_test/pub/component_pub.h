//
// Created by parallels on 9/7/16.
//

#ifndef RIAPS_FW_COMPONENT_PUB_H
#define RIAPS_FW_COMPONENT_PUB_H

#include "componentmodel/r_componentbase.h"
#include <string>
#include <czmq.h>



class component_pub : public riaps::ComponentBase {

public:

    component_pub(component_conf& config) : ComponentBase(config){

    };

    virtual void OnMessageArrived(std::string messagetype, zmsg_t* msg_body){

    }

    void init(){

    };

    ~component_pub(){
    };

};

#endif //RIAPS_FW_COMPONENT_PUB_H

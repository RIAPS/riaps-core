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

    void init(){

    };

    ~component_sub(){
    };

};

#endif //RIAPS_FW_COMPONENT_SUB_H

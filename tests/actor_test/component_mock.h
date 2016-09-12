//
// Created by parallels on 9/7/16.
//

#ifndef RIAPS_FW_COMPONENT_MOCK_H
#define RIAPS_FW_COMPONENT_MOCK_H

#include "componentmodel/r_componentbase.h"
#include <string>
#include <czmq.h>



class component_mock : public riaps::ComponentBase {

public:

    component_mock(component_conf& config) : ComponentBase(config){

    };

    void init(){

    };

    ~component_mock(){
    };

protected:
    //zactor_fn* actor_fn;
    //std::string name;
    //zactor_t* ca;
};

#endif //RIAPS_FW_COMPONENT_MOCK_H

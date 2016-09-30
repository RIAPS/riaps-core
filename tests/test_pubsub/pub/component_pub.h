//
// Created by parallels on 9/7/16.
//

#ifndef RIAPS_FW_COMPONENT_PUB_H
#define RIAPS_FW_COMPONENT_PUB_H

#include "componentmodel/r_componentbase.h"
#include <string>
#include <vector>
#include <czmq.h>

#include "../messagetypes/messages.h"



class component_pub : public riaps::ComponentBase {

public:

    component_pub(component_conf& config) : ComponentBase(config){
        counter = 0;
    };

    virtual void OnMessageArrived(std::string messagetype, zmsg_t* msg_body){

    }

    virtual void OnTimerFired(std::string timerid) {
        std::string istrmsg = "[" + std::to_string(counter++) + "]. Message from timer: " + timerid ;
        std::vector<std::string> params;
        params.push_back(istrmsg);
        zmsg_t* msg = create_message(MSG_ACT_PING, params);

        GetPublisherPorts()[0]->PublishMessage(&msg);

        //std::cout << istrmsg << std::endl;
    }

    void init(){

    };

    ~component_pub(){
    };

private:
    int counter;

};

#endif //RIAPS_FW_COMPONENT_PUB_H

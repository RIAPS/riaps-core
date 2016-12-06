//
// Created by istvan on 11/11/16.
//

#include "Sensor.h"
#include "common.h"

comp_sensor::comp_sensor(_component_conf_j &config, riaps::Actor &actor):ComponentBase(config, actor) {

}

void comp_sensor::OnMessageArrived(const std::string &messagetype, zmsg_t *msg_body,
                                   const riaps::ports::PortBase *port) {
    if (msg_body == NULL && port == NULL){

    }
    else {
        zmsg_t* msg = zmsg_new();
        zmsg_addstr(msg, "Data ready");
        std::cout << "on_clock(): " << messagetype <<std::endl;
        if (!SendMessageOnPort(msg, PORT_READY)) {
            std::cout << "Error sending message in timer" << std::endl;
        }
    }
}

comp_sensor::~comp_sensor() {

}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    return new comp_sensor(config, actor);
}



void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
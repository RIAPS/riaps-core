//
// Created by istvan on 11/11/16.
//

#include "Sensor.h"

comp_sensor::comp_sensor(_component_conf_j &config, riaps::Actor &actor):ComponentBase(config, actor) {

}

void comp_sensor::OnMessageArrived(std::string messagetype, zmsg_t *msg_body, zsock_t *socket) {

}

void comp_sensor::OnTimerFired(std::string timerid) {
    std::cout << "Timer fired: " + timerid <<std::endl;
}

comp_sensor::~comp_sensor() {

}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    return new comp_sensor(config, actor);
}



void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
//
// Created by istvan on 11/25/16.
//

#include "componentmodel/r_debugcomponent.h"

DebugComponent::DebugComponent(_component_conf_j &config, riaps::Actor &actor):ComponentBase(config, actor) {

}

void DebugComponent::OnMessageArrived(const std::string& messagetype, zmsg_t *msg_body, riaps::ports::PortBase *port) {
    if (msg_body == NULL && port == NULL){
        std::cout << "Timer event: " << messagetype << std::endl;
    }
    else {
        std::cout << "Debug component [" << _configuration.component_name << "](OnMessageArrived) "<< messagetype << std::endl;

    }
}

DebugComponent::~DebugComponent() {

}
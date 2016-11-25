//
// Created by istvan on 11/25/16.
//

#include "componentmodel/r_debugcomponent.h"

DebugComponent::DebugComponent(_component_conf_j &config, riaps::Actor &actor):ComponentBase(config, actor) {

}

//TODO: add OnPort into the cout message
void DebugComponent::OnMessageArrived(std::string messagetype, zmsg_t *msg_body, zsock_t *socket) {

    std::cout << "Debug component [" << _configuration.component_name << "](OnMessageArrived) "<< messagetype << std::endl;
}

void DebugComponent::OnTimerFired(std::string timerid) {
    std::cout << "Debug component [" << _configuration.component_name << "](OnTimerFired) "<< timerid << std::endl;
}

DebugComponent::~DebugComponent() {

}
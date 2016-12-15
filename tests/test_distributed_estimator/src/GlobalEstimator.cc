//
// Created by istvan on 11/11/16.
//


#include <GlobalEstimator.h>

comp_globalestimator::comp_globalestimator(_component_conf_j &config, riaps::Actor &actor):ComponentBase(config, actor) {

}

void comp_globalestimator::OnMessageArrived(const std::string& messagetype, zmsg_t* msg_body, const riaps::ports::PortBase* port) {

}

comp_globalestimator::~comp_globalestimator() {

}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    return new comp_globalestimator(config, actor);
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
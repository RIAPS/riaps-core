//
// Created by istvan on 11/11/16.
//

#include "LocalEstimator.h"

comp_localestimator::comp_localestimator(_component_conf_j &config, riaps::Actor &actor):ComponentBase(config, actor) {

}

void comp_localestimator::OnMessageArrived(std::string messagetype, zmsg_t *msg_body, zsock_t *socket) {
    std::cout << "on_ready(): " << messagetype << std::endl;
}

void comp_localestimator::OnTimerFired(std::string timerid) {

}

comp_localestimator::~comp_localestimator() {

}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    return new comp_localestimator(config, actor);
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
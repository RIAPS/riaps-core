//
// Created by istvan on 11/11/16.
//

#include "LocalEstimator.h"

comp_localestimator::comp_localestimator(_component_conf_j &config, riaps::Actor &actor):ComponentBase(config, actor) {

}

void comp_localestimator::OnMessageArrived(std::string messagetype, zmsg_t *msg_body, zsock_t *socket) {

    if (messagetype == PORT_READY) {

        char* msg_content = zmsg_popstr(msg_body);

        if (msg_content) {
            std::cout << "on_ready(): " << msg_content << std::endl;
            zstr_free(&msg_content);
        }

    }
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
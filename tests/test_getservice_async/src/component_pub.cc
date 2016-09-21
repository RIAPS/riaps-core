//
// Created by parallels on 9/21/16.
//

#include "component_pub.h"


component_pub::component_pub(component_conf &config) : ComponentBase(config) {

};

void component_pub::OnMessageArrived(std::string messagetype, zmsg_t *msg_body) {

}

void component_pub::OnTimerFired(std::string timerid) {

}

void init() {

};

component_pub::~component_pub() {
};


//
// Created by istvan on 9/30/16.
//

#include <prod/comp_gps.h>
#include "cons/comp_rep.h"

component_rep::component_rep(component_conf &config) : ComponentBase(config) {}

// TODO: destroy msg_body
void component_rep::OnMessageArrived(std::string messagetype, zmsg_t *msg_body, zsock_t* socket) {
    if (zsock_type(socket) == ZMQ_REP) {
        std::cout << "ZMQ::REP arrived" << std::endl;


        zstr_send(socket, "Arrived!");

    }
}

void component_rep::OnTimerFired(std::string timerid) {}


component_rep::~component_rep() {}
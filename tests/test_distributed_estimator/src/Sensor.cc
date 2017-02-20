//
// Created by istvan on 11/11/16.
//

#include "Sensor.h"
#
comp_sensor::comp_sensor(_component_conf_j &config, riaps::Actor &actor):ComponentBase(config, actor) {

}

void comp_sensor::OnMessageArrived(const std::string &messagetype, zmsg_t *msg_body,
                                   const riaps::ports::PortBase *port) {

    // port -> GetPortName() == messageType
    if (port->GetPortName() == PORT_CLOCK){
        std::cout << port->GetPortName() << std::endl;

        SendMessageOnPort("ready", PORT_READY);
    } else if (messagetype == PORT_REQUEST){
        if (msg_body){
            char* msgStr = zmsg_popstr(msg_body);

            if (msgStr){
                std::cout << "Arrived: " << msgStr <<std::endl;
                zstr_free(&msgStr);
            }
        }
    }
//    if (port->GetPortName() == "")
//
//    if (msg_body == NULL && port == NULL){
//
//    }
//    else {
//        zmsg_t* msg = zmsg_new();
//        zmsg_addstr(msg, "Data ready");
//        std::cout << "on_clock(): " << messagetype <<std::endl;
//        if (!SendMessageOnPort(msg, PORT_READY)) {
//            std::cout << "Error sending message in timer" << std::endl;
//        }
//    }
}

comp_sensor::~comp_sensor() {

}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    return new comp_sensor(config, actor);
}



void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
//
// Created by istvan on 11/11/16.
//

#include "Sensor.h"

comp_sensor::comp_sensor(_component_conf_j &config, riaps::Actor &actor):ComponentBase(config, actor) {

}

void comp_sensor::OnMessageArrived(const std::string &messagetype, zmsg_t *msg_body,
                                   riaps::ports::PortBase *port) {

    PrintMessageOnPort(port);

    // port -> GetPortName() == messageType is the same
    if (port->GetPortName() == PORT_CLOCK){

        SendMessageOnPort("ready", PORT_READY);
    } else if (messagetype == PORT_REQUEST){
        if (msg_body){
            char* msgStr = zmsg_popstr(msg_body);

            if (msgStr){
                zstr_free(&msgStr);

                auto rport = port->AsResponsePort();
                if (rport!=NULL){
                    rport->Send("Cool");
                }
            }
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
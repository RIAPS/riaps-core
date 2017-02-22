//
// Created by istvan on 11/11/16.
//

#include "Sensor.h"

comp_sensor::comp_sensor(_component_conf_j &config, riaps::Actor &actor):ComponentBase(config, actor) {

}

void comp_sensor::OnMessageArrived(const std::string& messagetype,
                                   std::vector<std::string>& msgFields,
                                   riaps::ports::PortBase* port) {

    PrintMessageOnPort(port);

    // port -> GetPortName() == messageType is the same
    if (port->GetPortName() == PORT_CLOCK){
        SendMessageOnPort(" -> msg <- ", PORT_READY);
    }

    else if (messagetype == PORT_REQUEST){
        riaps::ports::ResponsePort* repPort = GetResponsePortByName(PORT_READY);
        if (repPort != NULL) {
            if (repPort->Send("")) {
                std::cout << "Response on " << repPort->GetPortName() << " : " << port->AsRequestPort()->Recv();
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
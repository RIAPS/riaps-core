//
// Created by istvan on 11/11/16.
//

#include "LocalEstimator.h"



comp_localestimator::comp_localestimator(_component_conf_j &config, riaps::Actor &actor):ComponentBase(config, actor) {

}

void comp_localestimator::OnMessageArrived(const std::string& messagetype, zmsg_t* msg_body, const riaps::ports::PortBase* port) {

    if (msg_body == NULL && port == NULL){

    }
    else if (messagetype == PORT_READY) {

        char* msg_content = zmsg_popstr(msg_body);

        if (msg_content) {
            std::cout << "on_ready(): " << msg_content << std::endl;

            auto  port = GetPortByName(PORT_QUERY);

            if (port !=NULL && port->AsRequestPort()!=NULL){
                port->Send("Hello");
                std::cout << "Response on " << port->GetPortName() << " : " << port->AsRequestPort()->Recv();
            }



            zstr_free(&msg_content);
        }

    }
}

comp_localestimator::~comp_localestimator() {

}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    return new comp_localestimator(config, actor);
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
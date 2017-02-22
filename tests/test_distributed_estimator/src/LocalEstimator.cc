//
// Created by istvan on 11/11/16.
//

#include "LocalEstimator.h"



comp_localestimator::comp_localestimator(_component_conf_j &config, riaps::Actor &actor):ComponentBase(config, actor) {

}

void comp_localestimator::OnMessageArrived(const std::string& messagetype,
                                           std::vector<std::string>& msgFields,
                                           riaps::ports::PortBase* port) {

    PrintMessageOnPort(port);

    if (port->GetPortName() == PORT_READY) {
        auto reqPort = GetPortByName(PORT_QUERY);
        if (reqPort != NULL && reqPort->AsRequestPort() != NULL) {
            if (reqPort->Send("")) {
                std::cout << "Response on " << reqPort->GetPortName() << " : " << reqPort->AsRequestPort()->Recv();
            }
        }
    }
    
//    if (msg_body == NULL && port == NULL){
//
//    }
//    else if (messagetype == PORT_READY) {
//
//        char* msg_content = zmsg_popstr(msg_body);
//
//        if (msg_content) {
//
//            auto reqPort = GetPortByName(PORT_QUERY);
//            if (reqPort !=NULL && reqPort->AsRequestPort()!=NULL){
//                if (reqPort->Send("Hello")){
//                    std::cout << "Response on " << reqPort->GetPortName() << " : " << reqPort->AsRequestPort()->Recv();
//                }
//            }
//
//
//
//            zstr_free(&msg_content);
//        }
//
//    }
}

comp_localestimator::~comp_localestimator() {

}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    return new comp_localestimator(config, actor);
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
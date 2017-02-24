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

    if (port->GetPortName() == PORT_SUB_READY) {
        auto reqPort = GetRequestPortByName(PORT_REQ_QUERY);
        if (reqPort != NULL) {
            if (reqPort->Send("")) {
                std::string messageType;
                std::vector<std::string> messageFields;
                if (reqPort->AsRequestPort()->Recv(messageType, messageFields)){
                    std::string firstField = messageFields.front();
                    GetPublisherPortByName(PORT_PUB_ESTIMATE)->Send(firstField);
                }
            }
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
//
// Created by istvan on 3/10/17.
//

#include <base/LocalEstimatorBase.h>

LocalEstimatorBase::LocalEstimatorBase(_component_conf_j &config, riaps::Actor &actor) : ComponentBase(config, actor){

}

void LocalEstimatorBase::RegisterHandlers() {
    RegisterHandler(PORT_SUB_READY, reinterpret_cast<riaps::riaps_handler>(&LocalEstimatorBase::OnReady));
}

bool LocalEstimatorBase::SendQuery(const std::string &message) {
    return SendMessageOnPort(message, PORT_REQ_QUERY);
}

bool LocalEstimatorBase::SendEstimate(const std::string &message) {
    return SendMessageOnPort(message, PORT_PUB_ESTIMATE);
}

bool LocalEstimatorBase::RecvQuery(std::string& messageType, std::vector<std::string>& messageFields){

    auto reqPort = GetRequestPortByName(PORT_REQ_QUERY);
    if (reqPort != NULL) {
        // Wait for the response, and forward the message
        if (reqPort->AsRequestPort()->Recv(messageType, messageFields)) {
            //PrintMessageOnPort(reqPort);
            //td::string firstField = messageFields.front();
            //GetPublisherPortByName(PORT_PUB_ESTIMATE)->Send(firstField);
            return true;
        }
    }
    return false;
}

LocalEstimatorBase::~LocalEstimatorBase() {

}
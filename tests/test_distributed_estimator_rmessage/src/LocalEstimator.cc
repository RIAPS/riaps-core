//
// Created by istvan on 11/11/16.
//

#include "LocalEstimator.h"

namespace distributedestimator {
    namespace components {


        LocalEstimator::LocalEstimator(_component_conf_j &config, riaps::Actor &actor) : LocalEstimatorBase(config,
                                                                                                            actor) {
            //PrintParameters();
        }

        void LocalEstimator::OnReady(const std::string &messagetype,
                                     const messages::SensorReady &message,
                                     riaps::ports::PortBase *port) {

            PrintMessageOnPort(port);

            messages::SensorQuery queryMsg;
            SendQuery(queryMsg);

            std::string messageType;
            messages::SensorValue sensorValue;
            if (RecvQuery(messageType, sensorValue)) {
                std::cout << sensorValue.GetMsg() << std::endl;

                messages::Estimate estimateMessage;
                estimateMessage.SetMsg(sensorValue.GetMsg());
                SendEstimate(estimateMessage);
            }

            // Send the request
//    auto reqPort = GetRequestPortByName(PORT_REQ_QUERY);
//    if (reqPort != NULL) {
//        if (reqPort->Send("")) {
//            std::string messageType;
//            std::vector<std::string> messageFields;
//
//            // Wait for the response, and forward the message
//            if (reqPort->AsRequestPort()->Recv(messageType, messageFields)) {
//                PrintMessageOnPort(reqPort);
//                std::string firstField = messageFields.front();
//                GetPublisherPortByName(PORT_PUB_ESTIMATE)->Send(firstField);
//            }
//        }
//    }
        }


        LocalEstimator::~LocalEstimator() {

        }


    }
}

riaps::ComponentBase *create_component(_component_conf_j &config, riaps::Actor &actor) {
    auto result = new distributedestimator::components::LocalEstimator(config, actor);
    //result->RegisterHandlers();
    return result;
}

void destroy_component(riaps::ComponentBase *comp) {
    delete comp;
}
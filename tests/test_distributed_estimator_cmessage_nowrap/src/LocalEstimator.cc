//
// Created by istvan on 11/11/16.
//

#include "LocalEstimator.h"

namespace distributedestimator {
    namespace components {


        LocalEstimator::LocalEstimator(_component_conf_j &config, riaps::Actor &actor) :
                LocalEstimatorBase(config, actor) {
            //PrintParameters();
        }

        void LocalEstimator::OnReady(const std::string &messagetype,
                                     const messages::SensorReady::Reader &message,
                                     riaps::ports::PortBase *port) {

            PrintMessageOnPort(port);

            capnp::MallocMessageBuilder builderSensorQuery;

            messages::SensorQuery::Builder queryMsg = builderSensorQuery.initRoot<messages::SensorQuery>();

            queryMsg.setMsg("query");
            auto result = SendQuery(builderSensorQuery, queryMsg);
            if (result) {

                std::string messageType;
                messages::SensorValue::Reader sensorValue;
                if (RecvQuery(messageType, sensorValue)) {
                    std::cout << sensorValue.getMsg().cStr() << std::endl;
                    //messages::Estimate estimateMessage;
                    //estimateMessage.GetData().push_back(1.05);
                    //estimateMessage.GetData().push_back(10.05);
                    //SendEstimate(estimateMessage);
                }
            }
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
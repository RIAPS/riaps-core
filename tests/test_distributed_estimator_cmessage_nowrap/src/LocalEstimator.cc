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

        void LocalEstimator::OnReady(const messages::SensorReady::Reader &message,
                                     riaps::ports::PortBase *port) {

            //PrintMessageOnPort(port, message.getMsg().cStr());

            std::cout << "LocalEstimator::OnReady(): " << message.getMsg().cStr() << " " << ::getpid() << std::endl;

            capnp::MallocMessageBuilder builderSensorQuery;

            messages::SensorQuery::Builder queryMsg = builderSensorQuery.initRoot<messages::SensorQuery>();

            queryMsg.setMsg("sensor_query");
            auto result = SendQuery(builderSensorQuery, queryMsg);
            if (result) {
                messages::SensorValue::Reader sensorValue;
                if (RecvQuery(sensorValue)) {
                    std::cout << "LocalEstimator::OnQuery(): " << sensorValue.getMsg().cStr() << std::endl;
                    //std::cout << sensorValue.getMsg().cStr() << std::endl;
                    capnp::MallocMessageBuilder builderEstimate;
                    auto estimateMsg = builderEstimate.initRoot<messages::Estimate>();
                    estimateMsg.setMsg("local_est(" + std::to_string(::getpid()) + ")");
                    //auto valueList = estimateMsg.initValues(2);
                    //valueList.set(0, 1.05);
                    //valueList.set(1, 10.05);
                    SendEstimate(builderEstimate, estimateMsg);
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
    return result;
}

void destroy_component(riaps::ComponentBase *comp) {
    delete comp;
}
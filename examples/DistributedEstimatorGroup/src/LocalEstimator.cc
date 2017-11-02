//
// Created by istvan on 11/11/16.
//

#include "LocalEstimator.h"

namespace distributedestimator {
    namespace components {

        LocalEstimator::LocalEstimator(_component_conf &config, riaps::Actor &actor) :
                LocalEstimatorBase(config, actor) {
            //PrintParameters();
            hasJoined = false;
        }

        void LocalEstimator::OnReady(const messages::SensorReady::Reader &message,
                                     riaps::ports::PortBase *port) {

            //PrintMessageOnPort(port, message.getMsg().cStr());

            //std::cout << "LocalEstimator::OnReady(): " << message.getMsg().cStr() << " " << ::getpid() << std::endl;

            if (!hasJoined){
                hasJoined = true;
                if (this->JoinToGroup({"TestGroupId","Korte"})){
                    std::cout << "Joined to group TestGroupId::Korte" << std::endl;
                }


            }

            riaps::groups::GroupId gid;
            gid.groupTypeId = "TestGroupId";
            gid.groupName = "Korte";

            std::cout << "[LE] Count: " << GetGroupMemberCount(gid) << std::endl;

            
            capnp::MallocMessageBuilder builderSensorQuery;
            messages::SensorQuery::Builder queryMsg = builderSensorQuery.initRoot<messages::SensorQuery>();

            queryMsg.setMsg("sensor_query");
            auto result = SendQuery(builderSensorQuery, queryMsg);
            if (result) {
                messages::SensorValue::Reader sensorValue;
                if (RecvQuery(sensorValue)) {
                    //std::cout << "LocalEstimator::OnQuery(): " << sensorValue.getMsg().cStr() << std::endl;
                    //std::cout << sensorValue.getMsg().cStr() << std::endl;
                    capnp::MallocMessageBuilder builderEstimate;
                    auto estimateMsg = builderEstimate.initRoot<messages::Estimate>();
                    estimateMsg.setMsg("local_est(" + std::to_string(::getpid()) + ")");
                    SendEstimate(builderEstimate, estimateMsg);
                }
            }
        }

        void LocalEstimator::OnGroupMessage(const riaps::groups::GroupId &groupId,
                                            capnp::FlatArrayMessageReader &capnpreader, riaps::ports::PortBase *port) {
            std::cout << "[LE] Group message arrived!" << std::endl;
        }

        LocalEstimator::~LocalEstimator() {

        }
    }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
    auto result = new distributedestimator::components::LocalEstimator(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase *comp) {
    delete comp;
}
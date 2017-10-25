//
// Created by istvan on 11/11/16.
//


#include <GlobalEstimator.h>

namespace distributedestimator {
    namespace components {

        GlobalEstimator::GlobalEstimator(_component_conf &config, riaps::Actor &actor)
                : GlobalEstimatorBase(config, actor), _hasJoined(false) {
            //PrintParameters();
            auto param = GetConfig().component_parameters.GetParam("iArg");
            if (param!= nullptr){
                if (param->GetValueAsString() == "group"){
                    _hasGroup = true;
                }
            }
        }

        void GlobalEstimator::OnEstimate(messages::Estimate::Reader &message,
                                         riaps::ports::PortBase *port) {
            //PrintMessageOnPort(port);
            std::cout << "GlobalEstimator::OnEstimate(): " << message.getMsg().cStr() << std::endl;
            //std::cout << " " << message.getValues()[0] << " " << message.getValues()[1] << std::endl;
        }

        void GlobalEstimator::OnWakeup(riaps::ports::PortBase *port) {
            //PrintMessageOnPort(port);
            std::cout << "GlobalEstimator::OnWakeUp(): " << port->GetPortName() << std::endl;

            if (_hasGroup) {
                if (!_hasJoined) {
                    _hasJoined = true;
                    if (this->JoinToGroup({"TestGroupId", "Korte"})) {
                        std::cout << "Successfully joined to group TestGroupId::Korte" << std::endl;
                    }

                } else {
                    capnp::MallocMessageBuilder builder;
                    auto msgEstimate = builder.initRoot<distributedestimator::messages::Estimate>();
                    msgEstimate.setMsg("From group");

                    if (SendGroupMessage({"TestGroupId", "Korte"}, builder, "TestPubPortName")) {
                        std::cout << "[GE] Groupmessage sent" << std::endl;
                    } else
                        std::cout << "[GE] Groupmessage sending has been failed" << std::endl;
                }
            }
        }

        void GlobalEstimator::OnGroupMessage(const riaps::groups::GroupId &groupId,
                                             capnp::FlatArrayMessageReader &capnpreader,
                                             riaps::ports::PortBase *port) {
            std::cout << "[GE] Group message arrived!" << std::endl;

        }

        GlobalEstimator::~GlobalEstimator() {

        }
    }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
    auto result = new distributedestimator::components::GlobalEstimator(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase *comp) {
    delete comp;
}
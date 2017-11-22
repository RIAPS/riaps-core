//
// Created by istvan on 11/11/16.
//


#include <GlobalEstimator.h>

namespace distributedestimator {
    namespace components {

        GlobalEstimator::GlobalEstimator(_component_conf &config, riaps::Actor &actor)
                : GlobalEstimatorBase(config, actor), _hasJoined(false) {
            //PrintParameters();

            SetDebugLevel(_logger, spdlog::level::level_enum::debug);

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
            //std::cout << "GlobalEstimator::OnEstimate(): " << message.getMsg().cStr() << std::endl;
        }

        void GlobalEstimator::OnWakeup(riaps::ports::PortBase *port) {
            //PrintMessageOnPort(port);
            //_logger->debug("OnWakeUp()");

            if (_hasGroup) {
                if (!_hasJoined) {
                    _hasJoined = true;
                    bool rc = this->JoinToGroup({"TestGroupId", "Korte"});
                    _logger->debug_if(rc, "Successfully joined to group TestGroupId::Korte");

                } else {
//                    capnp::MallocMessageBuilder builder;
//                    auto msgEstimate = builder.initRoot<distributedestimator::messages::Estimate>();
//                    msgEstimate.setMsg("From group");
//
//                    if (SendGroupMessage({"TestGroupId", "Korte"}, builder, "TestPubPortName")) {
//                        std::cout << "[GE] Groupmessage sent" << std::endl;
//                    } else
//                        std::cout << "[GE] Groupmessage sending has been failed" << std::endl;
                }
            }
        }

        void GlobalEstimator::OnGroupMessage(const riaps::groups::GroupId &groupId,
                                             capnp::FlatArrayMessageReader &capnpreader,
                                             riaps::ports::PortBase *port) {
            //std::cout << "[GE] Group message arrived!" << std::endl;

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
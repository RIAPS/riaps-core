//
// Created by istvan on 11/11/16.
//

#ifndef RIAPS_FW_GLOBALESTIMATOR_H
#define RIAPS_FW_GLOBALESTIMATOR_H

#include "base/GlobalEstimatorBase.h"

namespace distributedestimator {
    namespace components {

        class GlobalEstimator : public GlobalEstimatorBase {

        public:

            GlobalEstimator(_component_conf &config, riaps::Actor &actor);

            virtual void OnEstimate(messages::Estimate::Reader &message,
                                    riaps::ports::PortBase *port);

            virtual void OnWakeup(riaps::ports::PortBase *port);

            virtual void OnGroupMessage(riaps::groups::GroupId& groupId,
                                        capnp::FlatArrayMessageReader* capnpreader);

            virtual bool SendGroupMessage(riaps::groups::GroupId&      groupId,
                                          capnp::MallocMessageBuilder& messageBuilder,
                                          const std::string&           portName);

            virtual ~GlobalEstimator();

        private:
            std::unique_ptr<std::uniform_real_distribution<double>> unif;
            std::default_random_engine re;
            bool hasJoined;
        };
    }
}

extern "C" riaps::ComponentBase* create_component(_component_conf&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);


#endif //RIAPS_FW_GLOBALESTIMATOR_H

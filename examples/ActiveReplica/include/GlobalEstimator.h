//
// Created by istvan on 11/11/16.
//

#ifndef RIAPS_FW_GLOBALESTIMATOR_H
#define RIAPS_FW_GLOBALESTIMATOR_H

#include "base/GlobalEstimatorBase.h"
#include <bitset>

#define QUERYID_LENGTH 32

namespace activereplica {
    namespace components {

        class GlobalEstimator : public GlobalEstimatorBase {

        public:

            GlobalEstimator(_component_conf &config, riaps::Actor &actor);

            virtual void OnEstimate(messages::Estimate::Reader &message,
                                    riaps::ports::PortBase *port);

            virtual void OnWakeup(riaps::ports::PortBase *port);

            void OnGroupMessage(const riaps::groups::GroupId& groupId,
                                capnp::FlatArrayMessageReader& capnpreader,
                                riaps::ports::PortBase* port);

            virtual ~GlobalEstimator();

        private:
            std::bitset<QUERYID_LENGTH> _pending;
            std::bitset<QUERYID_LENGTH> _idIndex;
            bool                        _hasJoined;
        };
    }
}

extern "C" riaps::ComponentBase* create_component(_component_conf&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);


#endif //RIAPS_FW_GLOBALESTIMATOR_H

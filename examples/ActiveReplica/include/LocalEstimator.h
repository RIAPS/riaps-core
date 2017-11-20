//
// Created by istvan on 11/11/16.
//

#ifndef RIAPS_FW_LOCALESTIMATOR_H
#define RIAPS_FW_LOCALESTIMATOR_H


#include "base/LocalEstimatorBase.h"

namespace activereplica {
    namespace components {

        class LocalEstimator : public LocalEstimatorBase {

        public:

            LocalEstimator(_component_conf &config, riaps::Actor &actor);

            virtual void OnReady(const messages::SensorReady::Reader &message,
                                 riaps::ports::PortBase *port);

            void OnGroupMessage(const riaps::groups::GroupId& groupId,
                                capnp::FlatArrayMessageReader& capnpreader,
                                riaps::ports::PortBase* port);

            virtual ~LocalEstimator();

        private:
            std::unique_ptr<double> _lastValue;
            bool                    _hasJoined;
        };
    }
}

extern "C" riaps::ComponentBase* create_component(_component_conf&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);


#endif //RIAPS_FW_LOCALESTIMATOR_H

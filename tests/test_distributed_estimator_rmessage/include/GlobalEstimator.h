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

            GlobalEstimator(_component_conf_j &config, riaps::Actor &actor);

            virtual void OnEstimate(const std::string &messagetype,
                                    messages::Estimate &message,
                                    riaps::ports::PortBase *port);

            virtual void OnWakeup(const std::string &messagetype,
                                  riaps::ports::PortBase *port);

            virtual ~GlobalEstimator();

        private:
            std::unique_ptr<std::uniform_real_distribution<double>> unif;
            std::default_random_engine re;
        };
    }
}

extern "C" riaps::ComponentBase* create_component(_component_conf_j&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);


#endif //RIAPS_FW_GLOBALESTIMATOR_H

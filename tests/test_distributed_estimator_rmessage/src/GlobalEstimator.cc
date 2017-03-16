//
// Created by istvan on 11/11/16.
//


#include <GlobalEstimator.h>

namespace distributedestimator {
    namespace components {


        GlobalEstimator::GlobalEstimator(_component_conf_j &config, riaps::Actor &actor) : GlobalEstimatorBase(config,
                                                                                                               actor) {
            //PrintParameters();
        }

        void GlobalEstimator::OnEstimate(const std::string &messagetype,
                                         const messages::Estimate &message,
                                         riaps::ports::PortBase *port) {
            PrintMessageOnPort(port);
        }

        void GlobalEstimator::OnWakeup(const std::string &messagetype,
                                       riaps::ports::PortBase *port) {
            PrintMessageOnPort(port);
        }

        GlobalEstimator::~GlobalEstimator() {

        }
    }

}

riaps::ComponentBase *create_component(_component_conf_j &config, riaps::Actor &actor) {
    auto result = new distributedestimator::components::GlobalEstimator(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase *comp) {
    delete comp;
}
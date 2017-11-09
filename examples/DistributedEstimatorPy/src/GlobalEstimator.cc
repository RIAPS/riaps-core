//
// Created by istvan on 11/11/16.
//


#include <GlobalEstimator.h>

namespace distributedestimator {
    namespace components {


        GlobalEstimator::GlobalEstimator(_component_conf &config, riaps::Actor &actor) : GlobalEstimatorBase(config,
                                                                                                             actor) {
            //PrintParameters();
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
        }

        void GlobalEstimator::OnOneShotTimer(const std::string& timerid){

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
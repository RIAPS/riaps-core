//
// Created by istvan on 11/11/16.
//


#include <GlobalEstimator.h>

#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace distributedestimator {
    namespace components {


        GlobalEstimator::GlobalEstimator(_component_conf &config, riaps::Actor &actor) : GlobalEstimatorBase(config,
                                                                                                             actor) {
        }

        void GlobalEstimator::OnEstimate(messages::Estimate::Reader &message,
                                         riaps::ports::PortBase *port) {
            //PrintMessageOnPort(port);
            _logger->info("GlobalEstimator::OnEstimate(): {}", message.getMsg().cStr());
        }

        void GlobalEstimator::OnWakeup(riaps::ports::PortBase *port) {
            //PrintMessageOnPort(port);
            _logger->info("GlobalEstimator::OnWakeUp(): {}", port->GetPortName());
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


PYBIND11_MODULE(ge, m) {
    py::class_<distributedestimator::components::GlobalEstimator> c(m, "GlobalEstimator");
    c.def(py::init());
}
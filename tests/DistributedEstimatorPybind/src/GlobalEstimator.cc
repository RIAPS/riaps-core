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

        GlobalEstimator::GlobalEstimator(
                        const py::object*  parent_actor ,
                        const py::dict     type_spec    ,
                        const std::string& name         ,
                        const std::string& type_name    ,
                        const py::dict     args)
                : GlobalEstimatorBase(parent_actor, type_spec, name, type_name, args){

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


std::unique_ptr<distributedestimator::components::GlobalEstimator> create_component_py(
        const py::object*  parent_actor ,
        const py::dict     type_spec    ,
        const std::string& name         ,
        const std::string& type_name    ,
        const py::dict     args
) {
    auto ptr = new distributedestimator::components::GlobalEstimator(parent_actor, type_spec, name, type_name, args);
    return std::move(std::unique_ptr<distributedestimator::components::GlobalEstimator>(ptr));
}

PYBIND11_MODULE(globalestimator, m) {
    py::class_<distributedestimator::components::GlobalEstimator> testClass(m, "GlobalEstimator");
    testClass.def(py::init<const py::object*, const py::dict, const std::string&, const std::string&, const py::dict>());
    testClass.def("setup", &distributedestimator::components::GlobalEstimator::setup);
    testClass.def("activate", &distributedestimator::components::GlobalEstimator::activate);
    m.def("create_component_py", &create_component_py, "Instantiates the component from python configuration");
}
//
// Created by istvan on 11/11/16.
//


#include <GlobalEstimator.h>

#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace distributedestimator {
    namespace components {
        GlobalEstimator::GlobalEstimator(const py::object *parent_actor,
                                         const py::dict actor_spec, // Actor json config
                                         const py::dict type_spec,  // component json config
                                         const std::string &name,
                                         const std::string &type_name,
                                         const py::dict args,
                                         const std::string &application_name,
                                         const std::string &actor_name)
                : GlobalEstimatorBase(parent_actor, actor_spec, type_spec, name, type_name, args, application_name,
                                      actor_name) {

        }

        void GlobalEstimator::OnEstimate(messages::Estimate::Reader &message,
                                         riaps::ports::PortBase *port) {
            component_logger()->info("GlobalEstimator::OnEstimate(): {}", message.getMsg().cStr());
        }

        void GlobalEstimator::OnWakeup(riaps::ports::PortBase *port) {
            component_logger()->info("GlobalEstimator::OnWakeUp(): {}", port->GetPortName());
        }



        GlobalEstimator::~GlobalEstimator() {

        }
    }

}

std::unique_ptr<distributedestimator::components::GlobalEstimator>
create_component_py(const py::object *parent_actor,
                    const py::dict actor_spec,
                    const py::dict type_spec,
                    const std::string &name,
                    const std::string &type_name,
                    const py::dict args,
                    const std::string &application_name,
                    const std::string &actor_name) {
    auto ptr = new distributedestimator::components::GlobalEstimator(parent_actor, actor_spec, type_spec, name, type_name, args,
                                                                     application_name,
                                                                     actor_name);
    return std::move(std::unique_ptr<distributedestimator::components::GlobalEstimator>(ptr));
}

PYBIND11_MODULE(libglobalestimator, m) {
    py::class_<distributedestimator::components::GlobalEstimator> testClass(m, "GlobalEstimator");
    testClass.def(py::init<const py::object*, const py::dict, const py::dict, const std::string&, const std::string&, const py::dict, const std::string&, const std::string&>());

    testClass.def("setup"                 , &distributedestimator::components::GlobalEstimator::Setup);
    testClass.def("activate"              , &distributedestimator::components::GlobalEstimator::Activate);
    testClass.def("handlePortUpdate"      , &distributedestimator::components::GlobalEstimator::HandlePortUpdate);
    testClass.def("handleCPULimit"        , &distributedestimator::components::GlobalEstimator::HandleCPULimit);
    testClass.def("handleMemLimit"        , &distributedestimator::components::GlobalEstimator::HandleMemLimit);
    testClass.def("handleSpcLimit"        , &distributedestimator::components::GlobalEstimator::HandleSpcLimit);
    testClass.def("handleNetLimit"        , &distributedestimator::components::GlobalEstimator::HandleNetLimit);
    testClass.def("handleNICStateChange"  , &distributedestimator::components::GlobalEstimator::HandleNICStateChange);
    testClass.def("handlePeerStateChange" , &distributedestimator::components::GlobalEstimator::HandlePeerStateChange);

    m.def("create_component_py", &create_component_py, "Instantiates the component from python configuration");
}
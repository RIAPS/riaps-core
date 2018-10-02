//
// Created by istvan on 3/10/17.
//

#include <base/GlobalEstimatorBase.h>
#include <componentmodel/r_pyconfigconverter.h>

namespace distributedestimator {
    namespace components {

//        GlobalEstimatorBase::GlobalEstimatorBase(_component_conf &config, riaps::Actor &actor) : ComponentBase(config,
//                                                                                                               actor) {
//
//        }


        GlobalEstimatorBase::GlobalEstimatorBase(const py::object *parent_actor,
                                                 const py::dict actor_spec, // Actor json config
                                                 const py::dict type_spec,  // component json config
                                                 const std::string &name,
                                                 const std::string &type_name,
                                                 const py::dict args,
                                                 const std::string &application_name,
                                                 const std::string &actor_name)
                : ComponentBase(application_name, actor_name) {

            auto conf = PyConfigConverter::convert(type_spec, actor_spec);
            conf.component_name = name;
            conf.component_type = type_name;
            conf.isDevice=false;
            set_config(conf);
        }

        void GlobalEstimatorBase::DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                                  riaps::ports::PortBase *port,
                                                  std::shared_ptr<riaps::MessageParams> params) {
            auto portName = port->GetPortName();
            if (portName == PORT_TIMER_WAKEUP) {
                OnWakeup(port);
            } else if (portName == PORT_SUB_ESTIMATE) {
                auto estimate = capnpreader->getRoot<messages::Estimate>();
                OnEstimate(estimate, port);
            }
        }

        void GlobalEstimatorBase::DispatchInsideMessage(zmsg_t *zmsg, riaps::ports::PortBase *port) {

        }

        GlobalEstimatorBase::~GlobalEstimatorBase() {

        }
    }
}
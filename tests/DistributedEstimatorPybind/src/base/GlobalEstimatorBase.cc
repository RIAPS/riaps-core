//
// Created by istvan on 3/10/17.
//

#include <base/GlobalEstimatorBase.h>
#include <converter.h>

namespace distributedestimator {
    namespace components {

        GlobalEstimatorBase::GlobalEstimatorBase(_component_conf &config, riaps::Actor &actor) : ComponentBase(config,
                                                                                                               actor) {

        }


        GlobalEstimatorBase::GlobalEstimatorBase(const py::object *parent_actor,
                                                 const py::dict type_spec,
                                                 const std::string &name,
                                                 const std::string &type_name,
                                                 const py::dict args)
                : ComponentBase() {
            auto result = PyConfigConverter::convert(type_spec);
            set_config(result);
            auto l = spd::stdout_color_mt("debuglogger");
            printNames<component_port_sub>(result.component_ports.subs);
            printNames<component_port_pub>(result.component_ports.pubs);
            printNames<component_port_req>(result.component_ports.reqs);
            printNames<component_port_rep>(result.component_ports.reps);
            printNames<component_port_tim>(result.component_ports.tims);
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
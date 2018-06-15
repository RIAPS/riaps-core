//
// Created by istvan on 3/10/17.
//

#include <base/GlobalEstimatorBase.h>

namespace distributedestimator {
    namespace components {

        GlobalEstimatorBase::GlobalEstimatorBase(_component_conf &config, riaps::Actor &actor) : ComponentBase(config,
                                                                                                               actor) {

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
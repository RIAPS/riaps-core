//
// Created by istvan on 3/10/17.
//

#include <base/GlobalEstimatorBase.h>

namespace distributedestimator {
    namespace components {

        GlobalEstimatorBase::GlobalEstimatorBase(_component_conf_j &config, riaps::Actor &actor) : ComponentBase(config,
                                                                                                                 actor) {

        }

        void GlobalEstimatorBase::DispatchMessage(const std::string &messagetype, capnp::FlatArrayMessageReader* capnpreader,
                                                  riaps::ports::PortBase *port) {
            auto portName = port->GetPortName();
            if (portName == PORT_TIMER_WAKEUP) {
                OnWakeup(messagetype, port);
            } else if (portName == PORT_SUB_ESTIMATE) {
                messages::Estimate estimate;
                estimate.InitReader(capnpreader);
                OnEstimate(messagetype, estimate, port);
            }
        }

        GlobalEstimatorBase::~GlobalEstimatorBase() {

        }
    }
}
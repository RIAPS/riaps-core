//
// Created by istvan on 3/10/17.
//

#include <base/GlobalEstimatorBase.h>

namespace distributedestimator {
    namespace components {

        GlobalEstimatorBase::GlobalEstimatorBase(_component_conf_j &config, riaps::Actor &actor) : ComponentBase(config,
                                                                                                                 actor) {

        }

        void GlobalEstimatorBase::DispatchMessage(const std::string &messagetype, riaps::MessageBase* message,
                                                  riaps::ports::PortBase *port) {
            auto portName = port->GetPortName();
            if (portName == PORT_TIMER_WAKEUP) {
                OnWakeup(messagetype, port);
            } else if (portName == PORT_SUB_ESTIMATE) {

                msgpack::unpacked msg;
                msgpack::unpack(&msg, message->data(), message->size());
                msgpack::object obj = msg.get();
                messages::Estimate estimate;
                obj.convert(&estimate);
                OnEstimate(messagetype, estimate, port);
                //OnReady(messagetype, sensorReady, port);
            }
        }

//void GlobalEstimatorBase::RegisterHandlers() {
//    RegisterHandler(PORT_SUB_ESTIMATE, reinterpret_cast<riaps::riaps_handler>(&GlobalEstimatorBase::OnEstimate));
//    RegisterHandler(PORT_TIMER_WAKEUP, reinterpret_cast<riaps::riaps_handler>(&GlobalEstimatorBase::OnWakeup));
//}

        GlobalEstimatorBase::~GlobalEstimatorBase() {

        }
    }
}
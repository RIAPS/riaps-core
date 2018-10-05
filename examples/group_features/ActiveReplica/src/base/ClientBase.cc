//
// Created by istvan on 3/10/17.
//

#include <base/ClientBase.h>

namespace activereplica {
    namespace components {

        ClientBase::ClientBase(_component_conf &config, riaps::Actor &actor) : ComponentBase(config,
                                                                                                               actor)
        {

        }

        void ClientBase::DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                         riaps::ports::PortBase *port,
                                          std::shared_ptr<riaps::MessageParams> payload
        ) {
            auto portName = port->GetPortName();
            if (portName == PORT_TIMER_WAKEUP) {
                OnWakeup(port);
            } else if (portName == PORT_SUB_ESTIMATE) {
                auto estimate = capnpreader->getRoot<messages::Estimate>();
                OnEstimate(estimate, port);
            }
        }

        void ClientBase::DispatchInsideMessage(zmsg_t *zmsg, riaps::ports::PortBase *port) {

        }

        ClientBase::~ClientBase() {

        }
    }
}
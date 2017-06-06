//
// Created by istvan on 3/10/17.
//

#include <base/ReceiverBase.h>

namespace timertest {
    namespace components {

        ReceiverBase::ReceiverBase(_component_conf_j &config, riaps::Actor &actor) : ComponentBase(config,
                                                                                                         actor) {

        }

        void ReceiverBase::DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                              riaps::ports::PortBase *port) {
            auto portName = port->GetPortName();
            if (portName == PORT_SUB_SIGNALVALUE) {
                auto signalValue = capnpreader->getRoot<messages::SignalValue>();
                OnSignalValue(signalValue, port);
            }

        }



        ReceiverBase::~ReceiverBase() {

        }

    }
}
//
// Created by istvan on 3/10/17.
//

#include <base/GeneratorBase.h>

namespace timertest {
    namespace components {

        GeneratorBase::GeneratorBase(_component_conf_j &config, riaps::Actor &actor) : ComponentBase(config,
                                                                                                         actor) {

        }

        void GeneratorBase::DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                              riaps::ports::PortBase *port) {
            auto portName = port->GetPortName();
            if (portName == PORT_TIMER_CLOCK) {
                OnClock(port);
            }

        }



        GeneratorBase::~GeneratorBase() {

        }

    }
}
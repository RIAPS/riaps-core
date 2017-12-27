//
// Created by istvan on 3/10/17.
//

#include <base/LeaderBase.h>

namespace leaderelection {
    namespace components {

        LeaderBase::LeaderBase(_component_conf &config, riaps::Actor &actor) : ComponentBase(config,
                                                                                                       actor) {

        }

        void LeaderBase::DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                              riaps::ports::PortBase *port,
                                              std::shared_ptr<riaps::MessageParams> params) {
            auto portName = port->GetPortName();
            if (portName == PORT_TIMER_CLOCK) {
                OnClock(port);
            }
        }

        void LeaderBase::DispatchInsideMessage(zmsg_t *zmsg, riaps::ports::PortBase *port) {

        }



        LeaderBase::~LeaderBase() {

        }

    }
}
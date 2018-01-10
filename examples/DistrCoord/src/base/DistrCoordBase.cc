//
// Created by istvan on 3/10/17.
//

#include <base/DistrCoordBase.h>

namespace dc {
    namespace components {

        DistrCoordBase::DistrCoordBase(_component_conf &config, riaps::Actor &actor) : ComponentBase(config,
                                                                                                       actor) {

        }

        void DistrCoordBase::DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                              riaps::ports::PortBase *port,
                                              std::shared_ptr<riaps::MessageParams> params) {
            auto portName = port->GetPortName();
            if (portName == PORT_TIMER_CLOCK) {
                OnClock(port);
            }
        }

        void DistrCoordBase::DispatchInsideMessage(zmsg_t *zmsg, riaps::ports::PortBase *port) {

        }



        DistrCoordBase::~DistrCoordBase() {

        }

    }
}
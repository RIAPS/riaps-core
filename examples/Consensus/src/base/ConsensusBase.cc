
#include <base/ConsensusBase.h>

namespace dc {
    namespace components {

        ConsensusBase::ConsensusBase(_component_conf &config, riaps::Actor &actor) : ComponentBase(config,
                                                                                                       actor) {

        }

        void ConsensusBase::DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                              riaps::ports::PortBase *port,
                                              std::shared_ptr<riaps::MessageParams> params) {
            auto portName = port->GetPortName();
            if (portName == PORT_TIMER_CLOCK) {
                OnClock(port);
            }
        }

        void ConsensusBase::DispatchInsideMessage(zmsg_t *zmsg, riaps::ports::PortBase *port) {

        }



        ConsensusBase::~ConsensusBase() {

        }

    }
}
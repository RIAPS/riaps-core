//
// Created by istvan on 6/20/17.
//

#include <base/EchoBase.h>

namespace echoio{
    namespace components{
        EchoBase::EchoBase(_component_conf_j &config, riaps::Actor &actor)
            : ComponentBase(config, actor){

        }

        bool EchoBase::SendEcho(capnp::MallocMessageBuilder &messageBuilder, messages::EchoRep::Builder &message) {
            return SendMessageOnPort(messageBuilder, PORT_REP_ECHO);
        }

        void EchoBase::DispatchMessage(capnp::FlatArrayMessageReader *capnpreader, riaps::ports::PortBase *port) {
            auto portName = port->GetPortName();
            if (portName == PORT_TIMER_CLOCK) {
                OnClock(port);
            } else if (portName == PORT_REP_ECHO) {
                auto sensorQuery = capnpreader->getRoot<messages::EchoReq>();
                OnEcho(sensorQuery, port);
            }
        }

        void EchoBase::DispatchInsideMessage(zmsg_t *zmsg, riaps::ports::PortBase *port) {

        }

        EchoBase::~EchoBase() {

        }
    }
}
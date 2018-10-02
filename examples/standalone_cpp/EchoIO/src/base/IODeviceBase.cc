//
// Created by istvan on 6/20/17.
//


#include <base/IODeviceBase.h>

namespace echoio {
    namespace components {
        IODeviceBase::IODeviceBase(_component_conf_j &config, riaps::Actor &actor)
             : ComponentBase(config, actor) {

        }

        bool IODeviceBase::SendEcho(capnp::MallocMessageBuilder &messageBuilder, messages::EchoReq::Builder &message) {
            return SendMessageOnPort(messageBuilder, PORT_REQ_ECHO);
        }

        bool IODeviceBase::SendTrigger(zmsg_t** message) {
            return SendMessageOnPort(message, INSIDE_TRIGGER);
        }

        void IODeviceBase::DispatchMessage(capnp::FlatArrayMessageReader *capnpreader, riaps::ports::PortBase *port) {
            auto portName = port->GetPortName();
            if (portName == PORT_TIMER_CLOCK) {
                OnClock(port);
            } else if (portName == PORT_REQ_ECHO) {
                auto echorep = capnpreader->getRoot<messages::EchoRep>();
                OnEcho(echorep, port);
            }
        }

        void IODeviceBase::DispatchInsideMessage(zmsg_t *zmsg, riaps::ports::PortBase *port) {
            auto portName = port->GetPortName();
            if (portName == INSIDE_TRIGGER) {
                OnTrigger(zmsg, port);
            }
        }

        IODeviceBase::~IODeviceBase() {

        }
    }
}
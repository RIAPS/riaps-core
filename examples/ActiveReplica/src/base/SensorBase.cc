//
// Created by istvan on 3/10/17.
//

#include <base/SensorBase.h>

namespace activereplica {
    namespace components {

        SensorBase::SensorBase(_component_conf &config, riaps::Actor &actor) : ComponentBase(config, actor) {

        }

        void SensorBase::DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                              riaps::ports::PortBase *port) {
            auto portName = port->GetPortName();
            if (portName == PORT_TIMER_CLOCK) {
                OnClock(port);
            } else if (portName == PORT_REP_REQUEST) {
                auto sensorQuery = capnpreader->getRoot<messages::SensorQuery>();
                OnRequest(sensorQuery, port);
            }

        }

        void SensorBase::DispatchInsideMessage(zmsg_t *zmsg, riaps::ports::PortBase *port) {

        }

        bool SensorBase::SendRequest(capnp::MallocMessageBuilder&    messageBuilder,
                                          messages::SensorValue::Builder& message) {
            return SendMessageOnPort(messageBuilder, PORT_REP_REQUEST);
        }

        bool SensorBase::SendReady(capnp::MallocMessageBuilder&    messageBuilder,
                                        messages::SensorReady::Builder& message) {
            return SendMessageOnPort(messageBuilder, PORT_PUB_READY);
        }

        SensorBase::~SensorBase() {

        }

    }
}
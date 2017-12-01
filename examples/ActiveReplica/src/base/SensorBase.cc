//
// Created by istvan on 3/10/17.
//

#include <base/SensorBase.h>

namespace activereplica {
    namespace components {

        SensorBase::SensorBase(_component_conf &config, riaps::Actor &actor) : ComponentBase(config, actor) {

        }

        void SensorBase::DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                         riaps::ports::PortBase *port,
                                         std::shared_ptr<riaps::MessageParams> params) {
            auto portName = port->GetPortName();
            if (portName == PORT_TIMER_CLOCK) {
                OnClock(port);
            } else if (portName == PORT_ANS_REQUEST) {
                auto sensorQuery = capnpreader->getRoot<messages::SensorQuery>();
                OnRequest(sensorQuery, port, params);
            }

        }

        void SensorBase::DispatchInsideMessage(zmsg_t *zmsg, riaps::ports::PortBase *port) {

        }

        bool SensorBase::SendRequest(capnp::MallocMessageBuilder&    messageBuilder,
                                     messages::SensorValue::Builder& message,
                                     std::shared_ptr<riaps::MessageParams> params) {
            return SendMessageOnPort(messageBuilder, PORT_ANS_REQUEST, params);
        }

        bool SensorBase::SendReady(capnp::MallocMessageBuilder&    messageBuilder,
                                        messages::SensorReady::Builder& message) {
            return SendMessageOnPort(messageBuilder, PORT_PUB_READY);
        }


    }
}
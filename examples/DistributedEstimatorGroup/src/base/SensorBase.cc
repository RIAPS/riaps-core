//
// Created by istvan on 3/10/17.
//

#include <base/SensorBase.h>

namespace distributedestimator {
    namespace components {

        comp_sensorbase::comp_sensorbase(_component_conf &config, riaps::Actor &actor) : ComponentBase(config,
                                                                                                       actor) {

        }

        void comp_sensorbase::DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                              riaps::ports::PortBase *port) {
            auto portName = port->GetPortName();
            if (portName == PORT_TIMER_CLOCK) {
                OnClock(port);
            } else if (portName == PORT_REP_REQUEST) {
                auto sensorQuery = capnpreader->getRoot<messages::SensorQuery>();
                OnRequest(sensorQuery, port);
            }

        }

        void comp_sensorbase::DispatchInsideMessage(zmsg_t *zmsg, riaps::ports::PortBase *port) {

        }

        bool comp_sensorbase::SendRequest(capnp::MallocMessageBuilder&    messageBuilder,
                                          messages::SensorValue::Builder& message) {
            return SendMessageOnPort(messageBuilder, PORT_REP_REQUEST);
        }

        bool comp_sensorbase::SendReady(capnp::MallocMessageBuilder&    messageBuilder,
                                        messages::SensorReady::Builder& message) {
            return SendMessageOnPort(messageBuilder, PORT_PUB_READY);
        }

        comp_sensorbase::~comp_sensorbase() {

        }

    }
}
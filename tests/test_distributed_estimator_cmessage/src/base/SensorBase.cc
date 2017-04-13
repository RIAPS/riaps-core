//
// Created by istvan on 3/10/17.
//

#include <base/SensorBase.h>

namespace distributedestimator {
    namespace components {

        comp_sensorbase::comp_sensorbase(_component_conf_j &config, riaps::Actor &actor) : ComponentBase(config,
                                                                                                         actor) {

        }

        void comp_sensorbase::DispatchMessage(const std::string &messagetype,
                                              capnp::FlatArrayMessageReader* capnpreader,
                                              riaps::ports::PortBase *port) {
            auto portName = port->GetPortName();
            if (portName == PORT_TIMER_CLOCK) {
                OnClock(messagetype, port);
            } else if (portName == PORT_REP_REQUEST) {
                messages::SensorQuery sensorQuery;
                sensorQuery.InitReader(capnpreader);
                OnRequest(messagetype, sensorQuery, port);
            }

        }

        bool comp_sensorbase::SendRequest(messages::SensorValue &message) {
            return SendMessageOnPort(&message, PORT_REP_REQUEST);
        }

        bool comp_sensorbase::SendReady(messages::SensorReady &message) {
            return SendMessageOnPort(&message, PORT_PUB_READY);
        }

        comp_sensorbase::~comp_sensorbase() {

        }

    }
}
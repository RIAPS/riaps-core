//
// Created by istvan on 11/11/16.
//

#include "Sensor.h"
#include <capnp/serialize.h>
#include <capnp/message.h>

namespace distributedestimator {
    namespace components {

        comp_sensor::comp_sensor(_component_conf &config, riaps::Actor &actor) : comp_sensorbase(config, actor) {
            //PrintParameters();
        }

        void comp_sensor::OnClock(riaps::ports::PortBase *port) {
            int64_t time = zclock_mono();
            _logger->info("Sensor::OnClock(): {}", time);

            capnp::MallocMessageBuilder messageBuilder;
            auto msgSensorReady = messageBuilder.initRoot<messages::SensorReady>();
            msgSensorReady.setMsg("data_ready");

            SendReady(messageBuilder, msgSensorReady);
        }

        void comp_sensor::OnRequest(const messages::SensorQuery::Reader &message,
                                    riaps::ports::PortBase *port) {

            //PrintMessageOnPort(port);
            if (port->GetPortBaseConfig()->isTimed){
                _logger->info("Sensor::OnRequest(): {}, sentTimestamp: {}.{}, recvTimestamp: {}.{}",
                              message.getMsg().cStr(),
                              port->AsResponsePort()->GetLastSendTimestamp().tv_sec ,
                              port->AsResponsePort()->GetLastSendTimestamp().tv_nsec,
                              port->AsResponsePort()->GetLastRecvTimestamp().tv_sec ,
                              port->AsResponsePort()->GetLastRecvTimestamp().tv_nsec);
            } else
                _logger->info("Sensor::OnRequest(): {}", message.getMsg().cStr());

            capnp::MallocMessageBuilder messageBuilder;
            messages::SensorValue::Builder msgSensorValue = messageBuilder.initRoot<messages::SensorValue>();
            msgSensorValue.setMsg("sensor_rep");

            if (!SendRequest(messageBuilder, msgSensorValue)){
                // Couldn't send the response
            }
        }



        comp_sensor::~comp_sensor() {

        }
    }
}

riaps::ComponentBase* create_component(_component_conf& config, riaps::Actor& actor){
    auto result = new distributedestimator::components::comp_sensor(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
//
// Created by istvan on 11/11/16.
//

#include "Sensor.h"
#include <capnp/serialize.h>
#include <capnp/message.h>

namespace distributedestimator {
    namespace components {

        comp_sensor::comp_sensor(_component_conf_j &config, riaps::Actor &actor) : comp_sensorbase(config, actor) {
            PrintParameters();
        }

        void comp_sensor::OnClock(riaps::ports::PortBase *port) {
            int64_t time = zclock_mono();
            std::cout << "Sensor::OnClock(): " << time << std::endl;

            capnp::MallocMessageBuilder messageBuilder;
            auto msgSensorReady = messageBuilder.initRoot<messages::SensorReady>();
            msgSensorReady.setMsg("data_ready");

            SendReady(messageBuilder, msgSensorReady);
        }

        void comp_sensor::OnRequest(const messages::SensorQuery::Reader &message,
                                    riaps::ports::PortBase *port) {
            //PrintMessageOnPort(port);

            //std::cout << "Sensor::OnRequest() " << message.getMsg().cStr() << std::endl;

            std::cout << "Sensor::OnRequest(): " << message.getMsg().cStr() <<std::endl;

            capnp::MallocMessageBuilder messageBuilder;
            messages::SensorValue::Builder msgSensorValue = messageBuilder.initRoot<messages::SensorValue>();
            msgSensorValue.setMsg("sensor_rep");

            if (!SendRequest(messageBuilder, msgSensorValue)){
                // Couldn't send the response
            }
        }

        void comp_sensor::OnOneShotTimer(const std::string &timerid) {

        }

        comp_sensor::~comp_sensor() {

        }
    }
}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    auto result = new distributedestimator::components::comp_sensor(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
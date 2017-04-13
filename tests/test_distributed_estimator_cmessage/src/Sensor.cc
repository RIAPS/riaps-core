//
// Created by istvan on 11/11/16.
//

#include "Sensor.h"

namespace distributedestimator {
    namespace components {

        comp_sensor::comp_sensor(_component_conf_j &config, riaps::Actor &actor) : comp_sensorbase(config, actor) {
            PrintParameters();
        }

        void comp_sensor::OnClock(const std::string &messagetype, riaps::ports::PortBase *port) {
            PrintMessageOnPort(port);
            messages::SensorReady readyMsg;
            readyMsg.SetMsg("ready");
            SendReady(readyMsg);
        }

        void comp_sensor::OnRequest(const std::string &messagetype, const messages::SensorQuery &message,
                                    riaps::ports::PortBase *port) {
            //PrintMessageOnPort(port);

            auto msg = message.GetMsg();
            std::cout << "Sensor::OnRequest() " << msg << std::endl;

            messages::SensorValue responseMessage;
            responseMessage.SetMsg("response");

            if (!SendRequest(responseMessage)){
                // Couldn't send the response
            }
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
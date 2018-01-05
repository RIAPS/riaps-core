//
// Created by istvan on 11/11/16.
//

#include "Sensor.h"
#include <capnp/serialize.h>
#include <capnp/message.h>

namespace activereplica {
    namespace components {

        Sensor::Sensor(_component_conf &config, riaps::Actor &actor) : SensorBase(config, actor) {
            _rndDistr = std::uniform_real_distribution<double>(-100.0, 100.0);  //(min, max)
            _rndEngine.seed(std::random_device{}());
        }

        void Sensor::OnClock(riaps::ports::PortBase *port) {
            _logger->info("OnClock()");

            capnp::MallocMessageBuilder messageBuilder;
            auto msgSensorReady = messageBuilder.initRoot<messages::SensorReady>();
            msgSensorReady.setMsg("data_ready");

            SendReady(messageBuilder, msgSensorReady);
        }

        void Sensor::OnRequest(const messages::SensorQuery::Reader &message,
                                    riaps::ports::PortBase *port) {
            capnp::MallocMessageBuilder messageBuilder;
            messages::SensorValue::Builder msgSensorValue = messageBuilder.initRoot<messages::SensorValue>();
            auto value = _rndDistr(_rndEngine);
            msgSensorValue.setValue(value);

            if (!SendRequest(messageBuilder, msgSensorValue)){
                // Couldn't send the response
                _logger->warn("Couldn't send message");
            } else{
                _logger->info("Sent: {}", value);
            }
        }

        void Sensor::OnGroupMessage(const riaps::groups::GroupId &groupId,
                                         capnp::FlatArrayMessageReader &capnpreader, riaps::ports::PortBase *port) {

        }


        bool Sensor::SendGroupMessage(riaps::groups::GroupId &groupId, capnp::MallocMessageBuilder &messageBuilder,
                                           const std::string &portName) {
            return true;
        }

        Sensor::~Sensor() {

        }
    }
}

riaps::ComponentBase* create_component(_component_conf& config, riaps::Actor& actor){
    auto result = new activereplica::components::Sensor(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
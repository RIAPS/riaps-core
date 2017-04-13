//
// Created by istvan on 4/13/17.
//

#include <base/TemperatureSensorBase.h>

namespace testing {
    namespace components {
        TemperatureSensorBase::TemperatureSensorBase(_component_conf_j &config, riaps::Actor &actor)
            : ComponentBase(config, actor){

        }

        bool TemperatureSensorBase::SendTemperature(capnp::MallocMessageBuilder &messageBuilder,
                                                    messages::SensorValue::Builder &message) {
            return SendMessageOnPort(messageBuilder, PORT_PUB_TEMPERATURE);
        }

        TemperatureSensorBase::~TemperatureSensorBase() {

        }
    }
}
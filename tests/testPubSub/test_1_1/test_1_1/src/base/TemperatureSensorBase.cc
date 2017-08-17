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

        void TemperatureSensorBase::DispatchMessage(capnp::FlatArrayMessageReader *capnpreader,
                                                    riaps::ports::PortBase *port) {
            auto portName = port->GetPortName();
            if (portName == PORT_TIMER_CLOCK) {
                OnClock(port);
            }
        }

		void TemperatureSensorBase::DispatchInsideMessage(zmsg_t*, riaps::ports::PortBase*)
		{
		}

        TemperatureSensorBase::~TemperatureSensorBase() {

        }
    }
}
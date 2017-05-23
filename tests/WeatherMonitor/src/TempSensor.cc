#include <TempSensor.h>

namespace weathermonitor {
    namespace components {		

        TempSensor::TempSensor(_component_conf_j &config, riaps::Actor &actor) :
                TempSensorBase(config, actor) {
			
			this->temperature = 65;
        }

		void TempSensor::OnClock(riaps::ports::PortBase *port) {
			std::cout << "TempSensor::OnClock(): " << port->GetPortName() << std::endl;
			
			this->temperature += 1
			capnp::MallocMessageBuilder messageBuilder;
			auto msgTempData = messageBuilder.initRoot<messages::TempData>();
			msgTempData.setMsg(this->temperature);
			
			if (!SendReady(messageBuilder, msgTempData)){
                std::cout << "TempSensor::OnClock(): Unable to publish temperature!" << std::endl;
            }
		}
		
		
		void TempSensor::OnOneShotTimer(const std::string& timerid){
		
		}

        TempSensor::~TempSensor() {

        }
    }
}

riaps::ComponentBase *create_component(_component_conf_j &config, riaps::Actor &actor) {
    auto result = new weathermonitor::components::TempSensor(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase *comp) {
    delete comp;
}

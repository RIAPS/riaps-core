//
// Created by istvan on 2/28/17.
//

#include <componentmodel/r_componentbase.h>
#include <TemperatureSensor.h>


namespace testing {
    namespace components{
        TemperatureSensor::TemperatureSensor(_component_conf_j &config, riaps::Actor &actor)
                : TemperatureSensorBase(config, actor)  {
            if ((GetConfig().component_parameters).GetParam(LOGFILEPATH)!=NULL){
                std::string logfile = GetConfig().component_parameters.GetParam(LOGFILEPATH)->GetValueAsString();
                _logfilePath = "/tmp/" + logfile;
            }
            else {
                std::cout << "Logfile parameter is not passed.";
            }

            _logStream.open(_logfilePath, std::fstream::out);
        }



        void TemperatureSensor::OnClock(riaps::ports::PortBase *port) {
            capnp::MallocMessageBuilder messageBuilder;
            auto msgSensorReady = messageBuilder.initRoot<messages::SensorValue>();
            msgSensorReady.setMsg(_messageCounter);

            SendTemperature(messageBuilder, msgSensorReady);
            _logStream << "Sent messages: " << _messageCounter++ << std::endl;
        }

        TemperatureSensor::~TemperatureSensor() {
            _logStream.close();
        }
    }
}


riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    return new testing::components::TemperatureSensor(config, actor);
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
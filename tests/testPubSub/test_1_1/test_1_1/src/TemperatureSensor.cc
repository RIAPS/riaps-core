//
// Created by istvan on 2/28/17.
//

#include <componentmodel/r_componentbase.h>
#include <TemperatureSensor.h>


namespace testing {
    namespace components{
        TemperatureSensor::TemperatureSensor(_component_conf_j &config, riaps::Actor &actor)
                : TemperatureSensorBase(config, actor)  {

            //PrintParameters();

            if ((GetConfig().component_parameters).GetParam(LOGFILEPATH)!=NULL){
                std::string logfile = GetConfig().component_parameters.GetParam(LOGFILEPATH)->GetValueAsString();
                _logfilePath = "/tmp/" + logfile;
            }
            else {
                std::cout << "Logfile parameter is not passed.";
            }

            _messageCounter = 0;

            _logStream.open(_logfilePath, std::fstream::out);
            //_logStream << "Logfile opened" << std::endl;
        }



        void TemperatureSensor::OnClock(riaps::ports::PortBase *port) {
            //_logStream << "OnClock " << std::endl;
            capnp::MallocMessageBuilder messageBuilder;
            auto msgSensorReady = messageBuilder.initRoot<messages::SensorValue>();
            msgSensorReady.setMsg(_messageCounter);

            //if (_messageCounter<10) {
                SendTemperature(messageBuilder, msgSensorReady);
                std::cout << "Sent messages: " << _messageCounter << std::endl;
                _logStream << "Sent messages: " << _messageCounter++ << std::endl;
                _logStream << std::flush;
            //} else if(_logStream.is_open()) {
                //_logStream.close();
            //}
        }
		
        void TemperatureSensor::OnOneShotTimer(const std::string& timerid){

        }			

        TemperatureSensor::~TemperatureSensor() {
            if(_logStream.is_open()) {
                _logStream.close();
            }
        }
    }
}


riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    return new testing::components::TemperatureSensor(config, actor);
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
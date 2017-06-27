//
// Created by istvan on 2/28/17.
//

#include <Collector.h>

namespace testing {
    namespace components{
        Collector::Collector(_component_conf_j &config, riaps::Actor &actor)
            : CollectorBase(config, actor)
        {
            //PrintParameters();
            if ((GetConfig().component_parameters).GetParam(LOGFILEPATH)!=NULL){
                std::string logfile = GetConfig().component_parameters.GetParam(LOGFILEPATH)->GetValueAsString();
                _logfilePath = "/tmp/" + logfile;
            }
            else {
                std::cout << "Logfile parameter is not passed.";
            }

            _logStream.open(_logfilePath, std::fstream::out);
        }

        void Collector::OnGetTemperature(testing::messages::SensorValue::Reader &message,
                                         riaps::ports::PortBase *port) {
            _logStream << "Received messages: " << message.getMsg() << std::endl;
            _logStream << std::flush;
//
//            if (message.getMsg()==9 && _logStream.is_open()){
//                _logStream.close();
//            }
        }
		
        void Collector::OnOneShotTimer(const std::string& timerid){

        }		

        Collector::~Collector() {
            if (_logStream.is_open())
                _logStream.close();
        }
    }
}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    return new testing::components::Collector(config, actor);
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
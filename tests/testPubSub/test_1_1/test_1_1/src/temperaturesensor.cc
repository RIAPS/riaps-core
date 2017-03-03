//
// Created by istvan on 2/28/17.
//

#include <componentmodel/r_componentbase.h>
#include <temperaturesensor.h>





TemperatureSensor::TemperatureSensor(_component_conf_j &config, riaps::Actor &actor):ComponentBase(config, actor) {
    if ((GetConfig().component_parameters).GetParam(LOGFILEPATH)!=NULL){
        std::string logfile = GetConfig().component_parameters.GetParam(LOGFILEPATH)->GetValueAsString();
        _logfilePath = "/tmp/" + logfile;
    }
    else {
        std::cout << "Logfile parameter is not passed.";
    }

    _logStream.open(_logfilePath, std::fstream::out);
}

void TemperatureSensor::OnMessageArrived(const std::string &messagetype, std::vector<std::string> &msgFields,
                                 riaps::ports::PortBase *port) {
    if (port->GetPortName() == PORT_TIMER_CLOCK){
        SendMessageOnPort(std::to_string(++_messageCounter), PORT_PUB_SENDTEMPERATURE);
        _logStream << "Sent messages: " << _messageCounter << std::endl;
    }
}

TemperatureSensor::~TemperatureSensor(){
    _logStream.close();
}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    return new TemperatureSensor(config, actor);
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
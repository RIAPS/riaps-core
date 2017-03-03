//
// Created by istvan on 2/28/17.
//

#include <collector.h>

Collector::Collector(_component_conf_j &config, riaps::Actor &actor):ComponentBase(config, actor) {
    if ((GetConfig().component_parameters).GetParam(LOGFILEPATH)!=NULL){
        std::string logfile = GetConfig().component_parameters.GetParam(LOGFILEPATH)->GetValueAsString();
        _logfilePath = "/tmp/" + logfile;
    }
    else {
        std::cout << "Logfile parameter is not passed.";
    }

    _logStream.open(_logfilePath, std::fstream::out);

}

void Collector::OnMessageArrived(const std::string &messagetype, std::vector<std::string> &msgFields,
                                 riaps::ports::PortBase *port) {

    if (port->GetPortName() == PORT_SUB_GETTEMPERATURE){
        _logStream << "Received messages: " << msgFields[0] << std::endl;
    }

}

Collector::~Collector(){
    _logStream.close();
}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    return new Collector(config, actor);
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
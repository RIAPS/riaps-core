//
// Created by istvan on 2/28/17.
//

#include <componentmodel/r_componentbase.h>
#include <temperaturesensor.h>

TemperatureSensor::TemperatureSensor(_component_conf_j &config, riaps::Actor &actor):ComponentBase(config, actor) {

}

void TemperatureSensor::OnMessageArrived(const std::string &messagetype, std::vector<std::string> &msgFields,
                                 riaps::ports::PortBase *port) {}

TemperatureSensor::~TemperatureSensor(){

}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    return new TemperatureSensor(config, actor);
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
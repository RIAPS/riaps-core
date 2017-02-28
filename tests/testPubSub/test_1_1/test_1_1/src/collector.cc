//
// Created by istvan on 2/28/17.
//

#include <collector.h>

Collector::Collector(_component_conf_j &config, riaps::Actor &actor):ComponentBase(config, actor) {

}

void Collector::OnMessageArrived(const std::string &messagetype, std::vector<std::string> &msgFields,
                                 riaps::ports::PortBase *port) {}

Collector::~Collector(){

}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    return new Collector(config, actor);
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
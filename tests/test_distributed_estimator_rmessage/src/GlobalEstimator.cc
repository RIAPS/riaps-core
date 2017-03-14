//
// Created by istvan on 11/11/16.
//


#include <GlobalEstimator.h>

GlobalEstimator::GlobalEstimator(_component_conf_j &config, riaps::Actor &actor):GlobalEstimatorBase(config, actor) {
    //PrintParameters();
}

void GlobalEstimator::OnEstimate(const std::string& messagetype,
                        std::vector<std::string>& msgFields,
                        riaps::ports::PortBase* port){
    PrintMessageOnPort(port);
}

void GlobalEstimator::OnWakeup(const std::string& messagetype,
                      std::vector<std::string>& msgFields,
                      riaps::ports::PortBase* port){
    PrintMessageOnPort(port);
}

GlobalEstimator::~GlobalEstimator() {

}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    auto result = new GlobalEstimator(config, actor);
    result->RegisterHandlers();
    return result;
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
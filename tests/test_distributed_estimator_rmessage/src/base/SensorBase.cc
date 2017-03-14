//
// Created by istvan on 3/10/17.
//

#include <base/SensorBase.h>

comp_sensorbase::comp_sensorbase(_component_conf_j &config, riaps::Actor &actor):ComponentBase(config, actor) {

}

void comp_sensorbase::RegisterHandlers() {

    RegisterHandler(PORT_REP_REQUEST, reinterpret_cast<riaps::riaps_handler>(&comp_sensorbase::OnRequest));
    RegisterHandler(PORT_TIMER_CLOCK, reinterpret_cast<riaps::riaps_handler>(&comp_sensorbase::OnClock));
}

bool comp_sensorbase::SendRequest(const std::string &message) {
    return SendMessageOnPort(message, PORT_REP_REQUEST);
}

bool comp_sensorbase::SendReady(const std::string &message) {
    return SendMessageOnPort(message, PORT_PUB_READY);
}

comp_sensorbase::~comp_sensorbase() {

}
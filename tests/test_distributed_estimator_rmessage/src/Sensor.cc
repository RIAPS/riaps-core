//
// Created by istvan on 11/11/16.
//

#include "Sensor.h"




comp_sensor::comp_sensor(_component_conf_j &config, riaps::Actor &actor):comp_sensorbase(config, actor) {
    PrintParameters();
}


void comp_sensor::OnClock(const std::string &messagetype, riaps::ports::PortBase *port) {
    PrintMessageOnPort(port);
    //SendMessageOnPort("ready message", PORT_PUB_READY);

    messages::SensorReady readyMsg;
    SendReady(readyMsg);
}

void comp_sensor::OnRequest(const std::string &messagetype, const messages::SensorQuery& message,
                            riaps::ports::PortBase *port) {
    PrintMessageOnPort(port);
    messages::SensorValue responseMessage;
    responseMessage.SetMsg("Kukucs");
    SendRequest(responseMessage);

    // Sync Request arrived, send response back
    //SendRequest("ResponseContent");
    //riaps::ports::ResponsePort* repPort = GetResponsePortByName(PORT_REP_REQUEST);
    //if (repPort != NULL) {
    //    if (repPort->Send("ResponseContent")) {
    //        //Success
    //    }
   // }
}

comp_sensor::~comp_sensor() {

}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    auto result = new comp_sensor(config, actor);
    //result->RegisterHandlers();
    return result;
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
//
// Created by istvan on 9/30/16.
//

#include "prod/comp_gps.h"

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    return new component_gps(config, actor);
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}

component_gps::component_gps(_component_conf_j& config, riaps::Actor& actor) : ComponentBase(config, actor) {
    double lower_bound = 0;
    double upper_bound = 10000;
    auto tmp = std::unique_ptr<std::uniform_real_distribution<double>>
            (new std::uniform_real_distribution<double>(lower_bound, upper_bound));

    unif = std::move(tmp);
}

// Sends GPS coordinates to subscribers
// Latitude, longitude
void component_gps::OnTimerFired(std::string timerid) {


    if (timerid == "GpsTimer") {

        double latitude = (*unif)(re);
        double longitude = (*unif)(re);

        std::vector<std::string> params;
        params.push_back(std::to_string(latitude));
        params.push_back(std::to_string(longitude));
        zmsg_t *msg = create_message(MSG_GPS, params);
        //
        GetPublisherPorts()[0]->PublishMessage(&msg);
    }
}

void component_gps::OnMessageArrived(std::string messagetype, zmsg_t *msg_body, zsock_t* socket) {

}

component_gps::~component_gps() {

}


//
// Created by istvan on 9/30/16.
//

#include "cons/comp_sub.h"

component_sub::component_sub(component_conf &config) : ComponentBase(config) {}

void component_sub::OnMessageArrived(std::string messagetype, zmsg_t *msg_body, zsock_t* socket) {
    if (messagetype == MSG_GPS) {
        char* latitude = zmsg_popstr(msg_body);
        char* longitude = zmsg_popstr(msg_body);

        //std::cout << "Latitude: " << latitude << std::endl;
        //std::cout << "Longitude: " << longitude << std::endl;
        //std::cout << std::endl;

        std::vector<std::string> params;
        params.push_back(std::string(latitude));
        params.push_back(std::string(longitude));

        auto reqmsg = create_message(MSG_GPS_REQ, params);

        auto response = this->GetRequestPorts()[0]->SendMessage(&reqmsg);



        free(latitude);
        free(longitude);

        //zmsg_destroy(msg_body);
    }
}

void component_sub::OnTimerFired(std::string timerid) {

}

component_sub::~component_sub() {}
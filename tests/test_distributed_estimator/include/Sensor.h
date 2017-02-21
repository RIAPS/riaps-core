//
// Created by istvan on 11/11/16.
//

#ifndef RIAPS_FW_SENSOR_H
#define RIAPS_FW_SENSOR_H

#include "componentmodel/r_componentbase.h"

#define PORT_CLOCK   "clock"
#define PORT_READY   "ready"
#define PORT_REQUEST "request"

class comp_sensor : public riaps::ComponentBase {

public:

    comp_sensor(_component_conf_j& config, riaps::Actor& actor);

    virtual void OnMessageArrived(const std::string& messagetype, zmsg_t* msg_body, riaps::ports::PortBase* port);


    virtual ~comp_sensor();

private:
    std::unique_ptr<std::uniform_real_distribution<double>> unif;
    std::default_random_engine                              re;
};

extern "C" riaps::ComponentBase* create_component(_component_conf_j&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);


#endif //RIAPS_FW_SENSOR_H

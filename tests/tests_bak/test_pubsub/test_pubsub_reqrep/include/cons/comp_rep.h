//
// Created by istvan on 9/30/16.
//

#ifndef RIAPS_FW_COMP_REQ_H
#define RIAPS_FW_COMP_REQ_H

#include "messages.h"
#include "componentmodel/r_componentbase.h"

#include <czmq.h>

#include <random>
#include <string>
#include <vector>


// Response component
class component_rep : public riaps::ComponentBase {

public:

    component_rep(_component_conf_j& config, riaps::Actor& actor);

    virtual void OnMessageArrived(std::string messagetype, zmsg_t* msg_body, zsock_t* socket);

    virtual void OnTimerFired(std::string timerid);

    virtual ~component_rep();

};

extern "C" riaps::ComponentBase* create_component(component_conf&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);


#endif //RIAPS_FW_COMP_REQ_H
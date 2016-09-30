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

    component_rep(component_conf& config);

    virtual void OnMessageArrived(std::string messagetype, zmsg_t* msg_body, zsock_t* socket);

    virtual void OnTimerFired(std::string timerid);

    virtual ~component_rep();

};


#endif //RIAPS_FW_COMP_REQ_H

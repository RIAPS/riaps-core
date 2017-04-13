//
// Created by istvan on 11/25/16.
//

#ifndef RIAPS_FW_R_DEBUGCOMPONENT_H
#define RIAPS_FW_R_DEBUGCOMPONENT_H

#include "r_componentbase.h"
#include "r_portbase.h"

// This component prints the arrived messages to the consol.
// Ports definitions come from the json file.
class DebugComponent : public riaps::ComponentBase {
public:
    DebugComponent(_component_conf_j& config, riaps::Actor& actor);

    virtual void OnMessageArrived(const std::string& messagetype, zmsg_t* msg_body, riaps::ports::PortBase* port);

    virtual ~DebugComponent();
};

#endif //RIAPS_FW_R_DEBUGCOMPONENT_H

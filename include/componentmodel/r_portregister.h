//
// Created by istvan on 9/22/17.
//

#ifndef RIAPS_CORE_R_PORTREGISTER_H
#define RIAPS_CORE_R_PORTREGISTER_H

#include "r_publisherport.h"
#include "r_subscriberport.h"
#include "r_responseport.h"
#include "r_requestport.h"
#include "r_insideport.h"

namespace riaps{
    class PortRegister{
    public:
        PortRegister();
    protected:
        const ports::PublisherPort*  InitPublisherPort(const _component_port_pub&);
        const ports::SubscriberPort* InitSubscriberPort(const _component_port_sub&);
        const ports::ResponsePort*   InitResponsePort(const _component_port_rep&);
        const ports::RequestPort*    InitRequestPort(const _component_port_req&);
        const ports::PeriodicTimer*  InitTimerPort(const _component_port_tim&);
        const ports::InsidePort*     InitInsiderPort(const _component_port_ins&);

        // All the component and group ports
        std::map<std::string, std::unique_ptr<ports::PortBase>> _ports;
    };
}

#endif //RIAPS_CORE_R_PORTREGISTER_H

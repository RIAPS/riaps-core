//
// Created by istvan on 9/22/17.
//

#ifndef RIAPS_CORE_R_PORTREGISTER_H
#define RIAPS_CORE_R_PORTREGISTER_H

#include "r_publisherport.h"

namespace riaps{

//    namespace ports {
//        class PublisherPort;
//        class SubscriberPort;
//        class ResponsePort;
//        class RequestPort;
//        class Plug;
//    }

    class PortRegister{
    public:
        PortRegister();
        virtual ~PortRegister();
    protected:

        const ports::PublisherPort*  InitPublisherPort(const _component_port_pub&);

        // All the component and group ports
        std::map<std::string, std::unique_ptr<ports::PortBase>> _ports;


    };
}

#endif //RIAPS_CORE_R_PORTREGISTER_H

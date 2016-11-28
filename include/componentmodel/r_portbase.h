//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_R_PORTBASE_H
#define RIAPS_R_PORTBASE_H

#include "r_configuration.h"

#include <czmq.h>
#include <string>
#include <iostream>

#define SERVICE_POLLING_INTERVAL 2000

namespace riaps {
namespace ports {

    enum PortTypes {Publisher, Subscriber, Request, Response};

    class PortBase {

    public:
        //PortBase(const ComponentBase* parentComponent);

        PortBase(PortTypes portType, component_port_config* config);

        /// \return The ip addres of the specified interface. (e.g.: "eth0")
        //virtual std::string GetInterfaceAddress(std::string ifacename);
        ///
        /// \return The ip address of the first ethernet interface
        virtual std::string GetInterfaceAddress();

        /// \return The associated ZMQ socket.
        virtual const zsock_t* GetSocket() const;

        virtual void Send(zmsg_t* msg) const;

        const PortTypes& GetPortType() const;

        virtual const component_port_config* GetConfig() const;


        ~PortBase();

    protected:
        PortTypes                    _port_type;
        zsock_t*                     _port_socket;
        std::string                  _port_name;
        const component_port_config* _config;
        //const ComponentBase* _parentComponent;
    };
}
}

#endif //RIAPS_R_PORTBASE_H

//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_R_PORTBASE_H
#define RIAPS_R_PORTBASE_H

#include "r_configuration.h"
#include "r_discoverdapi.h"

#include <czmq.h>
#include <iostream>

#define SERVICE_POLLING_INTERVAL 2000

namespace riaps {
    class PortBase {

    public:
        PortBase();

        /// \return The ip addres of the specified interface. (e.g.: "eth0")
        virtual std::string GetInterfaceAddress(std::string ifacename);
        ///
        /// \return The ip address of the first ethernet interface
        virtual std::string GetInterfaceAddress();

        /// \return The associated ZMQ socket.
        virtual const zsock_t* GetSocket();

        virtual ~PortBase();

    protected:
        zsock_t*       port_socket;

    };
}

#endif //RIAPS_R_PORTBASE_H

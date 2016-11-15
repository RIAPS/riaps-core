//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_R_PORTBASE_H
#define RIAPS_R_PORTBASE_H

#include <czmq.h>

#include <string>
#include <iostream>

#define SERVICE_POLLING_INTERVAL 2000

namespace riaps {

    class PortBase {

    public:
        //PortBase(const ComponentBase* parentComponent);

        //PortBase();

        /// \return The ip addres of the specified interface. (e.g.: "eth0")
        //virtual std::string GetInterfaceAddress(std::string ifacename);
        ///
        /// \return The ip address of the first ethernet interface
        virtual std::string GetInterfaceAddress();

        /// \return The associated ZMQ socket.
        virtual const zsock_t* GetSocket();


        ~PortBase();

    protected:
        zsock_t*              _port_socket;;
        //const ComponentBase* _parentComponent;
    };
}

#endif //RIAPS_R_PORTBASE_H

//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_R_PORTBASE_H
#define RIAPS_R_PORTBASE_H

#include "r_configuration.h"

#include <czmq.h>
#include <string>
#include <iostream>
//#include <componentmodel/r_responseport.h>
//#include <componentmodel/r_publisherport.h>
//#include <componentmodel/r_subscriberport.h>
//#include <componentmodel/r_requestport.h>

#define SERVICE_POLLING_INTERVAL 2000

namespace riaps {
namespace ports {

    class RequestPort;
    class ResponsePort;
    class PublisherPort;
    class SubscriberPort;
    class CallBackTimer;

    enum PortTypes {Publisher, Subscriber, Request, Response, Timer};

    class PortBase {

    public:
        //PortBase(const ComponentBase* parentComponent);

        PortBase(PortTypes portType, component_port_config* config);

        /// \return The ip addres of the specified interface. (e.g.: "eth0")
        //virtual std::string GetInterfaceAddress(std::string ifacename);
        ///
        /// \return The ip address of the first ethernet interface
        //virtual std::string GetInterfaceAddress();

        /// \return The associated ZMQ socket.
        virtual const zsock_t* GetSocket() const;

        virtual bool Send(zmsg_t** msg) const;

        virtual bool Send(std::string message) const;

        const PortTypes& GetPortType() const;

        virtual const component_port_config* GetConfig() const;

        virtual const std::string& GetPortName() const;

        // Return NULL if the called conversion is unavailable or invalid
        virtual RequestPort*    AsRequestPort();
        virtual PublisherPort*  AsPublishPort();
        virtual ResponsePort*   AsResponsePort();
        virtual SubscriberPort* AsSubscribePort();
        virtual CallBackTimer*  AsTimerPort();


        ~PortBase();

    protected:
        PortTypes                    _port_type;
        zsock_t*                     _port_socket;
        //std::string                  _port_name;
        const component_port_config* _config;
        //const ComponentBase* _parentComponent;
    };
}
}

#endif //RIAPS_R_PORTBASE_H

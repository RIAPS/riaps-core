//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_R_PORTBASE_H
#define RIAPS_R_PORTBASE_H

#include "r_configuration.h"

#include <czmq.h>
#include <string>
#include <iostream>

//#include <boost/format.hpp>
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
    class PeriodicTimer;

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


        bool Send(std::string message) const;
        bool Send(std::vector<std::string>& fields) const;
        virtual bool Send(zmsg_t** zmessage) const;

        const PortTypes& GetPortType() const;

        virtual const component_port_config* GetPortBaseConfig() const;

        virtual const std::string GetPortName() const;

        // Return NULL if the called conversion is unavailable or invalid
        virtual RequestPort*    AsRequestPort()    ;
        virtual PublisherPort*  AsPublishPort()    ;
        virtual ResponsePort*   AsResponsePort()   ;
        virtual SubscriberPort* AsSubscribePort()  ;
        virtual PeriodicTimer*  AsTimerPort();


        virtual ~PortBase() noexcept ;

    protected:



        PortTypes                    _port_type;
        zsock_t*                     _port_socket;

    private:
        //std::string                  _port_name;
        const component_port_config* _config;
        //const ComponentBase* _parentComponent;
    };
}
}

#endif //RIAPS_R_PORTBASE_H

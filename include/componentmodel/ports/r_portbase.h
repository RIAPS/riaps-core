
#ifndef RIAPS_R_PORTBASE_H
#define RIAPS_R_PORTBASE_H

#include <componentmodel/r_configuration.h>


#include <spdlog/spdlog.h>
#include <czmq.h>
#include <string>
#include <iostream>


#define SERVICE_POLLING_INTERVAL 2000

namespace spd = spdlog;

namespace riaps {

    class ComponentBase;

    namespace ports {

    class RequestPort;
    class ResponsePort;
    class PublisherPort;
    class SubscriberPort;
    class PeriodicTimer;
    class InsidePort;
    class GroupPublisherPort;
    class GroupSubscriberPort;
    class QueryPort;
    class AnswerPort;


    enum PortTypes {Publisher, Subscriber, Request, Response, Timer, Inside, Query, Answer};

    class PortBase {

    public:
        //PortBase(const ComponentBase* parentComponent);

        PortBase(PortTypes portType,
                 const component_port_config* config,
                 const ComponentBase* parentComponent);

        /// \return The ip addres of the specified interface. (e.g.: "eth0")
        //virtual std::string GetInterfaceAddress(std::string ifacename);
        ///
        /// \return The ip address of the first ethernet interface
        //virtual std::string GetInterfaceAddress();

        /// \return The associated ZMQ socket.
        virtual const zsock_t* GetSocket() const;


        //bool Send(std::string message) const;
        //bool Send(std::vector<std::string>& fields) const;
        //virtual bool Send(zmsg_t** zmessage) const;

        const ComponentBase* GetParentComponent();


        const PortTypes& GetPortType() const;

        virtual const component_port_config* GetPortBaseConfig() const;

        virtual const std::string GetPortName() const;

        // Return NULL if the called conversion is unavailable or invalid
        virtual RequestPort*         AsRequestPort()        ;
        virtual QueryPort*           AsQueryPort()          ;
        virtual PublisherPort*       AsPublishPort()        ;
        virtual GroupPublisherPort*  AsGroupPublishPort()   ;
        virtual GroupSubscriberPort* AsGroupSubscriberPort();
        virtual ResponsePort*        AsResponsePort()       ;
        virtual AnswerPort*          AsAnswerPort()  ;
        virtual SubscriberPort*      AsSubscribePort()      ;
        virtual PeriodicTimer*       AsTimerPort()          ;
        virtual InsidePort*          AsInsidePort()         ;


        virtual ~PortBase() noexcept ;

    protected:

        PortTypes                    _port_type;
        zsock_t*                     _port_socket;
        std::shared_ptr<spd::logger> _logger;


    private:
        const component_port_config* _config;
        const ComponentBase* _parentComponent;
    };
}
}

#endif //RIAPS_R_PORTBASE_H

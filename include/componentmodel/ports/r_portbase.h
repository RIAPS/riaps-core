
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
    class RecvPort;


    enum PortTypes {Publisher, Subscriber, Request, Response, Timer, Inside, Query, Answer};

    class PortBase {

    public:

        PortBase(PortTypes port_type,
                 const component_port_config* config,
                 const ComponentBase* parent_component);

        virtual const zsock_t*       port_socket()      const;
        const ComponentBase*         parent_component()      ;
        const PortTypes&             port_type()        const;
        const component_port_config* config()           const;
        const std::string            port_name()        const;

        // Return NULL if the called conversion is unavailable or invalid
        // TODO: Remove these, GetPortAs() does the job
        RequestPort*         AsRequestPort()        ;
        QueryPort*           AsQueryPort()          ;
        PublisherPort*       AsPublishPort()        ;
        GroupPublisherPort*  AsGroupPublishPort()   ;
        GroupSubscriberPort* AsGroupSubscriberPort();
        ResponsePort*        AsResponsePort()       ;
        AnswerPort*          AsAnswerPort()         ;
        SubscriberPort*      AsSubscribePort()      ;
        PeriodicTimer*       AsTimerPort()          ;
        InsidePort*          AsInsidePort()         ;
        RecvPort*            AsRecvPort()           ;

        template<class T>
        T* GetPortAs();

        virtual ~PortBase() noexcept ;

    protected:

        PortTypes                    port_type_;
        zsock_t*                     port_socket_;
        std::shared_ptr<spd::logger> logger_;


    private:
        const component_port_config* config_;
        const ComponentBase* parent_component_;
    };

    template<class T>
    T* PortBase::GetPortAs() {
        return dynamic_cast<T*>(this);
    }
}
}

#endif //RIAPS_R_PORTBASE_H


#ifndef RIAPS_R_PORTBASE_H
#define RIAPS_R_PORTBASE_H

#include <componentmodel/r_configuration.h>

#include <spdlog_setup/conf.h>

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
                 const ComponentPortConfig* config,
                 const ComponentBase* parent_component);

        virtual const zsock_t*       port_socket()      const;
        const ComponentBase*         parent_component()      ;
        const PortTypes&             port_type()        const;
        const ComponentPortConfig* config()           const;
        const std::string            port_name()        const;

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

        bool has_security() const;

        template<class T>
        T* GetPortAs();

        virtual ~PortBase() noexcept ;

    protected:
        PortTypes                    port_type_;
        zsock_t*                     port_socket_;
        std::shared_ptr<spd::logger> logger() const;

    private:
        const ComponentPortConfig* config_;
        const ComponentBase*       parent_component_;
        zcert_t*                   port_certificate_;
    };

    template<class T>
    T* PortBase::GetPortAs() {
        return dynamic_cast<T*>(this);
    }
}
}

#endif //RIAPS_R_PORTBASE_H

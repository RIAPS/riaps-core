
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

    /**
     * @brief Base class for all RIAPS ports. Stores metadata and the ZMQ port instance of the RIAPS port.
     * Provides API for the derived classes.
     */
    class PortBase {

    public:

        /// \cond
        PortBase(PortTypes port_type,
                 const ComponentPortConfig* config,
                 const ComponentBase* parent_component);
        ///\endcond

        /**
         * ZMQ socket of the port.
         * @return ZMQ socket.
         */
        virtual const zsock_t*       port_socket()      const;

        /**
         * Parent component.
         * @return Pointer to the component, which owns the port.
         */
        const ComponentBase*         parent_component()      ;

        /**
         * The type of the port.
         * @return The type of the port.
         */
        const PortTypes&             port_type()        const;

        /**
         * @return Object that contains all the properties of the port.
         */
        const ComponentPortConfig* config()           const;

        /**
         * Then name of the port.
         * @return The name of the port.
         */
        const std::string            port_name()        const;

        /// \cond
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
        /// \endcond

        /**
         * Security on/off.
         * @return True if the security is turned on. False otherwise.
         */
        bool has_security() const;

        /**
         * @tparam T RIAPS port type.
         * @return Returns the current port as T pointer.
         */
        template<class T>
        T* GetPortAs();

        virtual ~PortBase() noexcept ;

    protected:

        /**
         * Port type. Possible values:
         *  -# Publisher
         *  -# Subscriber
         *  -# Request
         *  -# Response
         *  -# Timer
         *  -# Inside
         *  -# Query
         *  -# Answer
         */
        PortTypes                    port_type_;

        /**
         * The ZMQ socket of the port.
         */
        zsock_t*                     port_socket_;

        /**
         * @return Logger for framework logging.
         */
        std::shared_ptr<spd::logger> logger() const;
        std::shared_ptr<zcert_t>   port_certificate_;

        /**
         * The applied certificate on the port when security is turned on.
         */
        std::shared_ptr<zcert_t>   port_certificate_;

    private:
        const ComponentPortConfig* config_;
        const ComponentBase*       parent_component_;
    };

    template<class T>
    T* PortBase::GetPortAs() {
        return dynamic_cast<T*>(this);
    }
}
}

#endif //RIAPS_R_PORTBASE_H

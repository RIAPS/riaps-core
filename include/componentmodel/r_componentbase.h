//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_R_COMPONENTBASE_H
#define RIAPS_R_COMPONENTBASE_H

#include "r_publisherport.h"
#include "r_subscriberport.h"
#include "r_discoverdapi.h"
#include "r_configuration.h"
#include "r_timer.h"
#include "r_responseport.h"
#include "r_requestport.h"

#include <iostream>
#include <vector>
#include <memory>
#include <queue>

namespace riaps {

    /**
     * @brief
     * @param pipe
     * @param args
     */
    void component_actor(zsock_t* pipe, void* args);

    class ComponentBase {
    public:
        //ComponentBase(component_conf& config);
        ComponentBase(component_conf_j& config);

        void AddPublisherPort(publisher_conf&);
        void AddSubscriberPort(std::unique_ptr<SubscriberPort>&);
        void AddResponsePort(std::unique_ptr<ResponsePort>&);
        void AddRequestPort(std::unique_ptr<RequestPort>&);
        void AddTimer(periodic_timer_conf&);

        std::vector<PublisherPort*>  GetPublisherPorts();
        std::vector<SubscriberPort*> GetSubscriberPorts();
        std::vector<CallBackTimer*>  GetPeriodicTimers();
        std::vector<ResponsePort*>   GetResponsePorts();
        std::vector<RequestPort*>    GetRequestPorts();

        //virtual std::vector<CallBackTimer*>  GetTimers();

        //virtual const zsock_t* GetTimerPort();

        std::string       GetTimerChannel();
        std::string       GetCompUuid();
        component_conf_j& GetConfig();

        virtual void OnMessageArrived(std::string messagetype, zmsg_t* msg_body, zsock_t* socket)=0;
        virtual void OnTimerFired(std::string timerid)=0;

        virtual ~ComponentBase();

    protected:
        component_conf_j _configuration;

        //std::string    async_address;
        zuuid_t*       _component_uuid;

        std::vector<std::unique_ptr<PublisherPort>>  _publisherports;
        //std::vector<std::unique_ptr<SubscriberPort>> _subscriberports;
        //std::vector<std::unique_ptr<CallBackTimer>>  _periodic_timers;
        //std::vector<std::unique_ptr<ResponsePort>>   _responseports;
        //std::vector<std::unique_ptr<RequestPort>>    _requestports;

        zactor_t*   _zactor_component;

        //zsock_t*    zsock_component;
        //zpoller_t*  zpoller;

    };
}

#endif //RIAPS_R_COMPONENTBASE_H

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
        ComponentBase(component_conf& config);

        void AddPublisherPort(publisher_conf&);
        void AddSubscriberPort(std::unique_ptr<SubscriberPort>&);
        void AddTimer(periodic_timer_conf&);

        virtual std::vector<PublisherPort*>  GetPublisherPorts();
        virtual std::vector<SubscriberPort*> GetSubscriberPorts();
        //virtual std::vector<CallBackTimer*>  GetTimers();

        //virtual const zsock_t* GetTimerPort();

        std::string     GetAsyncEndpointName();
        component_conf& GetConfig();

        virtual void OnMessageArrived(std::string messagetype, zmsg_t* msg_body)=0;
        virtual void OnTimerFired(std::string timerid)=0;

        virtual ~ComponentBase();

    protected:
        component_conf configuration;

        //std::string    async_address;
        zuuid_t*       component_uuid;

        std::vector<std::unique_ptr<PublisherPort>>  _publisherports;
        std::vector<std::unique_ptr<SubscriberPort>> _subscriberports;
        std::vector<std::unique_ptr<CallBackTimer>>  _periodic_timers;

        zactor_t*   zactor_component;
        //zsock_t*    zsock_component;
        //zpoller_t*  zpoller;

    };
}

#endif //RIAPS_R_COMPONENTBASE_H

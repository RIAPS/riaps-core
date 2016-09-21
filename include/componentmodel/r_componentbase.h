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

        // TODO: query endpoint of the component
        void AddPublisherPort(publisher_conf&);
        void AddSubscriberPort(subscriber_conf&);
        void AddTimer(periodic_timer_conf&);

        virtual std::vector<PublisherPort*>  GetPublisherPorts();
        virtual std::vector<SubscriberPort*> GetSubscriberPorts();
        //virtual std::vector<CallBackTimer*>  GetTimers();

        virtual const zsock_t* GetTimerPort();
        virtual const zsock_t* GetAsyncPort();

        virtual void OnMessageArrived(std::string messagetype, zmsg_t* msg_body)=0;
        virtual void OnTimerFired(std::string timerid)=0;

        virtual ~ComponentBase();

    protected:
        component_conf configuration;

        std::vector<std::unique_ptr<PublisherPort>>  _publisherports;
        std::vector<std::unique_ptr<SubscriberPort>> _subscriberports;
        std::vector<std::unique_ptr<CallBackTimer>>  _periodic_timers;

        zactor_t*  zactor_component;
        zsock_t*   zsock_component;
        zsock_t*   _zsock_async_result;
        zsock_t*   _zsock_timer;
        zpoller_t* zpoller;

    };
}

#endif //RIAPS_R_COMPONENTBASE_H

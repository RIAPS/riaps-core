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
#include "r_actor.h"

#include <iostream>
#include <vector>
#include <memory>
#include <queue>
#include <random>


namespace riaps {

    class Actor;
    class PublisherPort;
    class SubscriberPort;

    /**
     * @brief
     * @param pipe
     * @param args
     */
    void component_actor(zsock_t* pipe, void* args);

    class ComponentBase {
    public:
        //ComponentBase(component_conf& config);
        ComponentBase(component_conf_j& config, Actor& actor);

        void InitPublisherPort(_component_port_pub_j&);
        void InitSubscriberPort(_component_port_sub_j&);
        //void AddResponsePort(std::unique_ptr<ResponsePort>&);
        //void AddRequestPort(std::unique_ptr<RequestPort>&);
        void AddTimer(_component_port_tim_j&);

        std::vector<PublisherPort*>  GetPublisherPorts();
        std::vector<SubscriberPort*> GetSubscriberPorts();

        SubscriberPort& GetSubscriberByName(const std::string&);


        //std::vector<CallBackTimer*>  GetPeriodicTimers();
        //std::vector<ResponsePort*>   GetResponsePorts();
        //std::vector<RequestPort*>    GetRequestPorts();

        //virtual std::vector<CallBackTimer*>  GetTimers();

        //virtual const zsock_t* GetTimerPort();

        std::string       GetTimerChannel();
        std::string       GetCompUuid();
        const component_conf_j& GetConfig() const;

        const Actor* GetActor() const;

        virtual void OnMessageArrived(std::string messagetype, zmsg_t* msg_body, zsock_t* socket)=0;
        virtual void OnTimerFired(std::string timerid)=0;

        virtual ~ComponentBase();

    protected:
        const Actor*     _actor;
        component_conf_j _configuration;

        //std::string    async_address;
        zuuid_t*       _component_uuid;

        std::vector<std::unique_ptr<PublisherPort>>  _publisherports;
        std::vector<std::unique_ptr<SubscriberPort>> _subscriberports;
        //std::vector<std::unique_ptr<CallBackTimer>>  _periodic_timers;
        //std::vector<std::unique_ptr<ResponsePort>>   _responseports;
        //std::vector<std::unique_ptr<RequestPort>>    _requestports;

        zactor_t*   _zactor_component;

        //zsock_t*    zsock_component;
        //zpoller_t*  zpoller;

    };
}

#endif //RIAPS_R_COMPONENTBASE_H

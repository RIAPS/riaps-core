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

    namespace ports {
        class PublisherPort;
        class SubscriberPort;
        class ResponsePort;
    }

    /**
     * @brief
     * @param pipe
     * @param args
     */
    void component_actor(zsock_t* pipe, void* args);

    class ComponentBase {
    public:
        ComponentBase(component_conf_j& config, Actor& actor);

        const ports::PublisherPort*  InitPublisherPort(const _component_port_pub_j&);
        const ports::SubscriberPort* InitSubscriberPort(const _component_port_sub_j&);
        const ports::ResponsePort*   InitResponsePort(const _component_port_rep_j&);

        void AddTimer(_component_port_tim_j&);

        std::vector<ports::PublisherPort*>  GetPublisherPorts();
        std::vector<ports::SubscriberPort*> GetSubscriberPorts();

        ports::SubscriberPort& GetSubscriberByName(const std::string&);

        ports::PortBase* GetPortByName(const std::string&);

        bool SendMessageOnPort(zmsg_t* msg, std::string portName) const;


        //std::vector<CallBackTimer*>  GetPeriodicTimers();
        //std::vector<ResponsePort*>   GetResponsePorts();
        //std::vector<RequestPort*>    GetRequestPorts();

        //virtual std::vector<CallBackTimer*>  GetTimers();

        //virtual const zsock_t* GetTimerPort();

        std::string       GetTimerChannel();
        std::string       GetCompUuid();
        const component_conf_j& GetConfig() const;

        const Actor* GetActor() const;
        zactor_t* GetZmqPipe() const;

        virtual void OnMessageArrived(const std::string& messagetype, zmsg_t* msg_body, const ports::PortBase* port)=0;

        virtual ~ComponentBase();

    protected:
        const ports::PortBase* GetPort(std::string portName) const;

        const Actor*     _actor;
        component_conf_j _configuration;

        //std::string    async_address;
        zuuid_t*       _component_uuid;

        //std::map<std::string, std::unique_ptr<PublisherPort>>  _publisherports;
        //std::vector<std::unique_ptr<ports::SubscriberPort>> _subscriberports;
        std::vector<std::unique_ptr<CallBackTimer>>  _periodic_timers;

        std::map<std::string, std::unique_ptr<ports::PortBase>> _ports;


        //std::vector<std::unique_ptr<ResponsePort>>   _responseports;
        //std::vector<std::unique_ptr<RequestPort>>    _requestports;

        zactor_t*   _zactor_component;

        //zsock_t*    zsock_component;
        //zpoller_t*  zpoller;

    private:


    };
}

#endif //RIAPS_R_COMPONENTBASE_H

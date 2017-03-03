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
        class RequestPort;
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
        const ports::RequestPort*    InitRequestPort(const _component_port_req_j&);
        const ports::CallBackTimer*  InitTimerPort(const _component_port_tim_j&);

        ports::PublisherPort*  GetPublisherPortByName(const std::string& portName);
        ports::RequestPort*    GetRequestPortByName(const std::string& portName);
        ports::ResponsePort*   GetResponsePortByName(const std::string& portName);
        ports::SubscriberPort* GetSubscriberPortByName(const std::string& portName);


        ports::PortBase* GetPortByName(const std::string&);

        bool SendMessageOnPort(std::string message, std::string portName);


        std::string             GetTimerChannel();
        std::string             GetCompUuid();
        const component_conf_j& GetConfig() const;

        const Actor* GetActor() const;
        zactor_t* GetZmqPipe() const;

        virtual void OnMessageArrived(const std::string& messagetype,
                                      std::vector<std::string>& msgFields,
                                      ports::PortBase* port)=0;

        virtual void PrintMessageOnPort(ports::PortBase* port, std::string message="");
        virtual void PrintParameters();

        virtual ~ComponentBase();

    protected:
        const ports::PortBase* GetPort(std::string portName) const;
        const Actor*     _actor;
        zuuid_t*       _component_uuid;

        zactor_t*   _zactor_component;

    private:
        // Reach the configuration by GetConfig(), never ever directly.
        component_conf_j _configuration;

        // All the component ports
        std::map<std::string, std::unique_ptr<ports::PortBase>> _ports;

    };
}

#endif //RIAPS_R_COMPONENTBASE_H

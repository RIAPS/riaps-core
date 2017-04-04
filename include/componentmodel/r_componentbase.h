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
#include "r_messagebase.h"

#include <msgpack.hpp>
#include <capnp/message.h>
#include <capnp/serialize.h>

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

    typedef void (riaps::ComponentBase::*riaps_handler)(const std::string&, msgpack::sbuffer*, riaps::ports::PortBase*);

    class ComponentBase {
    public:
        ComponentBase(component_conf_j& config, Actor& actor);

        const Actor* GetActor() const;
        zactor_t* GetZmqPipe() const;

        friend void component_actor(zsock_t* pipe, void* args);

        bool SendMessageOnPort(std::string message, const std::string& portName);
        bool SendMessageOnPort(zmsg_t** message, const std::string& portName);
        bool SendMessageOnPort(msgpack::sbuffer& message, const std::string& portName);
        bool SendMessageOnPort(MessageBase* message, const std::string& portName);
        bool SendMessageOnPort(capnp::MallocMessageBuilder& message, const std::string& portName);

        const component_conf_j& GetConfig() const;

        virtual void PrintMessageOnPort(ports::PortBase* port, std::string message="");
        virtual void PrintParameters();

        virtual ~ComponentBase();

    protected:
        const ports::PortBase* GetPort(std::string portName) const;
        virtual void RegisterHandler(const std::string& portName, riaps_handler);

        ports::PublisherPort*  GetPublisherPortByName(const std::string& portName);
        ports::RequestPort*    GetRequestPortByName(const std::string& portName);
        ports::ResponsePort*   GetResponsePortByName(const std::string& portName);
        ports::SubscriberPort* GetSubscriberPortByName(const std::string& portName);

        ports::PortBase* GetPortByName(const std::string&);

/*        virtual void DispatchMessage(const std::string& messagetype,
                                     msgpack::sbuffer* message,
                                     ports::PortBase* port);
*/
        /*virtual void DispatchMessage(const std::string& messagetype,
                                     riaps::MessageBase* message,
                                     ports::PortBase* port) = 0;
*/

//        virtual void DispatchMessage(const std::string& messagetype,
//                                     kj::ArrayPtr<const capnp::word>* data,
//                                     ports::PortBase* port) = 0;

        virtual void DispatchMessage(const std::string& messagetype,
                                     capnp::FlatArrayMessageReader* capnpreader,
                                     ports::PortBase* port) = 0;

        const Actor* _actor;
        zuuid_t*     _component_uuid;
        zactor_t*    _zactor_component;

    private:
        const ports::PublisherPort*  InitPublisherPort(const _component_port_pub_j&);
        const ports::SubscriberPort* InitSubscriberPort(const _component_port_sub_j&);
        const ports::ResponsePort*   InitResponsePort(const _component_port_rep_j&);
        const ports::RequestPort*    InitRequestPort(const _component_port_req_j&);
        const ports::CallBackTimer*  InitTimerPort(const _component_port_tim_j&);

        std::string             GetTimerChannel();
        std::string             GetCompUuid();

        virtual riaps_handler GetHandler(std::string portName);



        // Reach the configuration by GetConfig(), never ever directly.
        component_conf_j _configuration;

        // All the component ports
        std::map<std::string, std::unique_ptr<ports::PortBase>> _ports;
        std::map<std::string, riaps_handler> _handlers;
    };
}




#endif //RIAPS_R_COMPONENTBASE_H

//
// Created by parallels on 9/7/16.
//

#include <componentmodel/r_configuration.h>
#include <componentmodel/ports/r_portbase.h>
#include <componentmodel/r_componentbase.h>

#include <capnp/message.h>
#include <fmt/format.h>

namespace riaps {


    namespace ports {


        PortBase::PortBase(PortTypes portType,
                           const component_port_config* config,
                           const ComponentBase* parentComponent)
                : _parentComponent(parentComponent) {
            _port_type = portType;
            _config = config;
            _port_socket = nullptr;

            // InsidePorts have no parent components
            if (parentComponent == nullptr) {
                std::string loggerPrefix = portType == PortTypes::Inside?"InsidePort":"NullParent";
                std::string loggerName = fmt::format("{}::{}", loggerPrefix, config->portName);
                _logger = spd::stdout_color_mt(loggerName);
            } else {
                _logger = spd::get(_parentComponent->GetConfig().component_name);
            }



        }

//        bool PortBase::Send(capnp::MallocMessageBuilder &message) const {
//            return false;
//        }


//        bool PortBase::Send(zmsg_t** zmessage) const {
//            return false;
//            //throw std::runtime_error("ZMQ message cannot be sent on this port port : " + GetPortBaseConfig()->portName);
//        }
//
//        bool PortBase::Send(std::string message) const{
//            zmsg_t* zmsg = zmsg_new();
//            zmsg_addstr(zmsg, message.c_str());
//
//            return Send(&zmsg);
//        }
//
//        bool PortBase::Send(std::vector<std::string>& fields) const {
//            zmsg_t* zmsg = zmsg_new();
//
//            for (auto it = fields.begin(); it!=fields.end(); it++){
//                zmsg_addstr(zmsg, it->c_str());
//            }
//
//            return Send(&zmsg);
//        }

        const zsock_t *PortBase::GetSocket() const {
            return _port_socket;
        }

        const ComponentBase* PortBase::GetParentComponent() {
            return _parentComponent;
        }

        const component_port_config* PortBase::GetPortBaseConfig() const {
            return _config;
        }


        const PortTypes& PortBase::GetPortType() const {
            return _port_type;
        }

        const std::string PortBase::GetPortName() const {
            return _config->portName;
        }

        RequestPort* PortBase::AsRequestPort()  {
            return nullptr;
        }

        PublisherPort* PortBase::AsPublishPort()  {
            return nullptr;
        }

        GroupPublisherPort* PortBase::AsGroupPublishPort() {
            return nullptr;
        }

        GroupSubscriberPort* PortBase::AsGroupSubscriberPort() {
            return nullptr;
        }

        ResponsePort* PortBase::AsResponsePort()  {
            return nullptr;
        }

        SubscriberPort* PortBase::AsSubscribePort()  {
            return nullptr;
        }

        PeriodicTimer* PortBase::AsTimerPort()  {
            return nullptr;
        }

        InsidePort* PortBase::AsInsidePort()  {
            return nullptr;
        }

        QueryPort* PortBase::AsQueryPort() {
            return nullptr;
        }

        AnswerPort* PortBase::AsAnswerPort() {
            return nullptr;
        }

        PortBase::~PortBase() {
            if (_port_socket) {
                // std::cout << "Destroy port socket : " << GetPortName() << std::endl;
                zsock_destroy(&_port_socket);
            }
        }
    }
}
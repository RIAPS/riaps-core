//
// Created by parallels on 9/7/16.
//

#include <componentmodel/r_configuration.h>
#include <componentmodel/ports/r_portbase.h>
#include <componentmodel/r_componentbase.h>

#include <capnp/message.h>
#include <fmt/format.h>

using namespace std;

namespace riaps {
    namespace ports {

        PortBase::PortBase(PortTypes port_type,
                           const component_port_config* config,
                           const ComponentBase* parent_component)
                : parent_component_(parent_component) {
            port_type_ = port_type;
            config_ = config;
            port_socket_ = nullptr;

            // InsidePorts have no parent components
            if (parent_component == nullptr) {
                string logger_prefix = port_type_ == PortTypes::Inside?"InsidePort":"NullParent";
                string logger_name = fmt::format("{}::{}", logger_prefix, config->port_name);
                logger_ = spd::stdout_color_mt(logger_name);
            } else {
                logger_ = spd::get(parent_component_->component_config().component_name);
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
            return port_socket_;
        }

        const ComponentBase* PortBase::parent_component() {
            return parent_component_;
        }

        const component_port_config* PortBase::GetPortBaseConfig() const {
            return config_;
        }


        const PortTypes& PortBase::port_type() const {
            return port_type_;
        }

        const std::string PortBase::port_name() const {
            return config_->port_name;
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

        RecvPort* PortBase::AsRecvPort() {
            return nullptr;
        }

        PortBase::~PortBase() {
            if (port_socket_) {
                zsock_destroy(&port_socket_);
            }
        }
    }
}
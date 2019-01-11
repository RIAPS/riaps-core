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
                           const ComponentPortConfig* config,
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

        const zsock_t *PortBase::port_socket() const {
            return port_socket_;
        }

        const ComponentBase* PortBase::parent_component() {
            return parent_component_;
        }

        const ComponentPortConfig* PortBase::config() const {
            return config_;
        }


        const PortTypes& PortBase::port_type() const {
            return port_type_;
        }

        const std::string PortBase::port_name() const {
            return config_->port_name;
        }

        RequestPort* PortBase::AsRequestPort()  {
            return GetPortAs<RequestPort>();
        }

        PublisherPort* PortBase::AsPublishPort()  {
            return GetPortAs<PublisherPort>();
        }

        GroupPublisherPort* PortBase::AsGroupPublishPort() {
            return GetPortAs<GroupPublisherPort>();
        }

        GroupSubscriberPort* PortBase::AsGroupSubscriberPort() {
            return GetPortAs<GroupSubscriberPort>();
        }

        ResponsePort* PortBase::AsResponsePort()  {
            return GetPortAs<ResponsePort>();
        }

        SubscriberPort* PortBase::AsSubscribePort()  {
            return GetPortAs<SubscriberPort>();
        }

        PeriodicTimer* PortBase::AsTimerPort()  {
            return GetPortAs<PeriodicTimer>();
        }

        InsidePort* PortBase::AsInsidePort()  {
            return GetPortAs<InsidePort>();
        }

        QueryPort* PortBase::AsQueryPort() {
            return GetPortAs<QueryPort>();
        }

        AnswerPort* PortBase::AsAnswerPort() {
            return GetPortAs<AnswerPort>();
        }

        RecvPort* PortBase::AsRecvPort() {
            return GetPortAs<RecvPort>();
        }

        PortBase::~PortBase() {
            if (port_socket_) {
                zsock_destroy(&port_socket_);
            }
        }
    }
}
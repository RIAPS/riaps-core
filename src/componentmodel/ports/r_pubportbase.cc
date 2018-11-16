//
// Created by istvan on 10/4/17.
//

#include <componentmodel/ports/r_pubportbase.h>
#include <framework/rfw_network_interfaces.h>

namespace riaps{
    namespace ports{

        PublisherPortBase::PublisherPortBase(const component_port_config* config, const ComponentBase* parent)
                : PortBase(PortTypes::Publisher, config, parent),
                  SenderPort(this) {

        }

        PublisherPortBase::~PublisherPortBase(){

        }

        void PublisherPortBase::InitSocket() {
            port_socket_ = zsock_new(ZMQ_PUB);

            if (GetConfig()->is_local){
                host_ = "127.0.0.1";
            } else {
                host_ = riaps::framework::Network::GetIPAddress();
            }

            if (host_ == "") {
                throw std::runtime_error("Publisher cannot be initiated. Cannot find  available network interface.");
            }

            std::string pub_endpoint = "tcp://" + host_ + ":!";
            port_ = zsock_bind(port_socket_, "%s", pub_endpoint.c_str());


            if (port_ == -1) {
                throw std::runtime_error("Couldn't bind publisher port.");
            }

            logger_->debug("Publisher is created on {}:{} [{}]", host_, port_, GetConfig()->message_type);
        }

        const component_port_pub* PublisherPortBase::GetConfig() const {
            return (component_port_pub*)GetPortBaseConfig();
        }

        std::string PublisherPortBase::GetEndpoint() {
            if (port_socket_) {
                return std::string(zsock_endpoint(port_socket_));
            }
            return "";
        }
    }
}
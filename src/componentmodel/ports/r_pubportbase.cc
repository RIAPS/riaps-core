//
// Created by istvan on 10/4/17.
//

#include <componentmodel/ports/r_pubportbase.h>
#include <framework/rfw_network_interfaces.h>

using namespace std;

namespace riaps{
    namespace ports{

        PublisherPortBase::PublisherPortBase(const ComponentPortConfig* config, const ComponentBase* parent)
                : PortBase(PortTypes::Publisher, config, parent),
                  SenderPort(this) {

        }

        void PublisherPortBase::InitSocket() {
            port_socket_ = zsock_new(ZMQ_PUB);

            if (GetConfig()->is_local){
                host_ = "127.0.0.1";
            } else {
                host_ = riaps::framework::Network::GetIPAddress();
            }

            if (host_ == "") {
                logger()->error("Publisher cannot be initiated. Cannot find  available network interface.");
            }

            string pub_endpoint = fmt::format("tcp://{}:!",host_);
            port_ = zsock_bind(port_socket_, "%s", pub_endpoint.c_str());

            if (port_ == -1) {
                logger()->error("Couldn't bind publisher port: {}", port_name());
            } else
                logger()->debug("Publisher is created on {}:{} [{}]", host_, port_, GetConfig()->message_type);
        }

        const ComponentPortPub* PublisherPortBase::GetConfig() const {
            return (ComponentPortPub*) config();
        }

        std::string PublisherPortBase::endpoint() {
            if (port_socket_) {
                return std::string(zsock_endpoint(port_socket_));
            }
            return "";
        }
    }
}
//
// Created by istvan on 10/4/17.
//

#include <componentmodel/ports/r_pubportbase.h>
#include <framework/rfw_network_interfaces.h>
#include <framework/rfw_security.h>

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
                auto iface = riaps::framework::Network::GetConfiguredIface();
                host_ = riaps::framework::Network::GetIPAddress(iface);
            }

            if (host_ == "") {
                logger()->error("Publisher cannot be initiated. Cannot find  available network interface.");
            }

            // The port is NOT local AND encrypted
            //if (!GetConfig()->is_local && has_security()) {
            if (has_security()) {
//                zactor_t *auth = zactor_new (zauth, NULL);
//                auth_ = shared_ptr<zactor_t>(auth, [](zactor_t* z) {zactor_destroy(&z);});
//                zstr_sendx (auth, "VERBOSE", NULL);
//                zsock_wait (auth);
//                zstr_sendx (auth, "CURVE", CURVE_FOLDER, NULL);
//                zsock_wait (auth);
////                zstr_sendx (auth, "ALLOW", "192.168.1.104", "192.168.1.101", NULL);
////                zsock_wait (auth);

                if (port_certificate_ != nullptr) {
                    zcert_apply (port_certificate_.get(), port_socket_);
                    zsock_set_curve_server (port_socket_, 1);
                } else {
                    logger()->error("Port certificate is null, cannot create port: {}", port_name());
                    return;
                }
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
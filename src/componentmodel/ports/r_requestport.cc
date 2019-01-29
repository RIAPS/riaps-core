#include <componentmodel/ports/r_requestport.h>
#include <framework/rfw_network_interfaces.h>

using namespace std;
using namespace riaps::discovery;

namespace riaps {
    namespace ports {

        RequestPort::RequestPort(const ComponentPortReq &config, const ComponentBase *parent)
                : PortBase(PortTypes::Request, (ComponentPortConfig*)(&config), parent),
                  SenderPort(this),
                  RecvPort(this),
                  capnp_reader_(capnp::FlatArrayMessageReader(nullptr)) {
            port_socket_ = zsock_new(ZMQ_REQ);
            int timeout = 500;//msec
            int linger = 0;
            int connect_timeout = 1000; //msec
            zsock_set_sndtimeo(port_socket_, timeout);
            zsock_set_rcvtimeo(port_socket_, timeout);
            zsock_set_linger(port_socket_, linger);

            is_connected_ = false;
        }

        void RequestPort::Init() {
            const ComponentPortReq* current_config = GetConfig();
            const string host = (current_config->is_local) ? "127.0.0.1" : riaps::framework::Network::GetIPAddress();

            if (!GetConfig()->is_local && has_security()) {
                if (port_certificate_ != nullptr) {
                    zcert_apply(port_certificate_.get(), port_socket_);
                    zsock_set_curve_serverkey(port_socket_, zcert_public_txt(port_certificate_.get()));
                } else {
                    logger()->error("Port certificate is null, cannot create port: {}", port_name());
                    return;
                }
            }

            auto results =
                    Disco::SubscribeToService(
                            parent_component()->actor()->application_name(),
                            parent_component()->component_config().component_name,
                            parent_component()->actor()->actor_name(),
                            host,
                            riaps::discovery::Kind::REQ,
                            (current_config->is_local ? riaps::discovery::Scope::LOCAL
                                                      : riaps::discovery::Scope::GLOBAL),
                            current_config->port_name,
                            current_config->message_type);

            for (auto& result : results) {
                string endpoint = fmt::format("tcp://{}:{}", result.host_name, result.port);
                ConnectToResponse(endpoint);
            }
        }

        bool RequestPort::ConnectToResponse(const std::string &rep_endpoint) {
            int rc = zsock_connect(port_socket_, "%s", rep_endpoint.c_str());

            if (rc != 0) {
                logger()->error("Request '{}' couldn't connect to ", GetConfig()->port_name, rep_endpoint);
                return false;
            }

            is_connected_ = true;
            logger()->debug("Request port connected to: {}", rep_endpoint);
            return true;
        }

        const ComponentPortReq* RequestPort::GetConfig() const{
            return (ComponentPortReq*) config();
        }

//        const timespec& RequestPort::recv_timestamp() const {
//            return recv_timestamp_;
//        }

        PortError RequestPort::Send(capnp::MallocMessageBuilder &message) const{
            if (!port_socket_)
                this->logger()->error("Port socket ({}) is null: {} ", port_name(), __func__);

            return SenderPort::Send(message);
        }

        RequestPort::~RequestPort() noexcept {

        }
    }
}

#include <componentmodel/ports/r_subportbase.h>

using namespace std;

namespace riaps {
    namespace ports{
        SubscriberPortBase::SubscriberPortBase(const ComponentPortConfig *config, const ComponentBase* parent)
                : PortBase(PortTypes::Subscriber, config, parent), RecvPort(this) {
           InitSocket();
        }

        void SubscriberPortBase::InitSocket() {
            port_socket_ = zsock_new(ZMQ_SUB);
            assert(port_socket_);
            zsock_set_subscribe(port_socket_, "");
        }

        /**
         * @param endpoint The endpoint, INCLUDING the transport layer. e.g.: tcp://192.168.1.1:4245
         * @return True if the connection successful. False otherwise.
         */
        bool SubscriberPortBase::ConnectToPublihser(const string &endpoint) {
            if (!GetConfig()->is_local && has_security()) {
                if (port_certificate_ != nullptr) {
                    zcert_apply(port_certificate_.get(), port_socket_);
                    zsock_set_curve_serverkey(port_socket_, zcert_public_txt(port_certificate_.get()));
                } else {
                    logger()->error("Port certificate is null, cannot create port: {}", port_name());
                    return false;
                }
            }

            auto rc = zsock_connect(port_socket_, "%s", endpoint.c_str());
            if (rc != 0) {
                logger()->error("Subscriber {} couldn't connect to {}", port_name(), endpoint);
                return false;
            }

            logger()->debug("Subscriber connected to: {} [{}]", endpoint, GetConfig()->message_type);
            return true;
        }

        const ComponentPortSub* SubscriberPortBase::GetConfig() const{
            return (ComponentPortSub*) config();
        }

        SubscriberPortBase::~SubscriberPortBase() {

        }
    }
}
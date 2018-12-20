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
         *
         * @param endpoint The endpoint, INCLUDING the transport layer. e.g.: tcp://192.168.1.1:4245
         * @return True if the connection successful. False otherwise.
         */
        bool SubscriberPortBase::ConnectToPublihser(const string &endpoint) {
            int rc = zsock_connect(port_socket_, "%s", endpoint.c_str());

            if (rc != 0) {
                // TODO: spd logger
                cout << "Subscriber '" + config()->port_name + "' couldn't connect to " + endpoint
                          << endl;
                return false;
            }

            logger_->debug("Subscriber connected to: {} [{}]", endpoint, GetConfig()->message_type);
            return true;
        }

        const ComponentPortSub* SubscriberPortBase::GetConfig() const{
            return (ComponentPortSub*) config();
        }

        SubscriberPortBase::~SubscriberPortBase() {

        }
    }
}
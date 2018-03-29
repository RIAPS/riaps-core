//
// Created by istvan on 10/17/17.
//

#include <componentmodel/ports/r_subportbase.h>

namespace riaps {
    namespace ports{
        SubscriberPortBase::SubscriberPortBase(const component_port_config *config, const ComponentBase* parentComponent)
                : PortBase(PortTypes::Subscriber, config, parentComponent), RecvPort(this) {
           InitSocket();
        }

        void SubscriberPortBase::InitSocket() {
            m_port_socket = zsock_new(ZMQ_SUB);
            assert(m_port_socket);
            zsock_set_subscribe(m_port_socket, "");
        }

        /// \param pub_endpoint The endpoint, INCLUDING the transport layer. e.g.: tcp://192.168.1.1:4245
        /// \return True if the connection successful. False otherwise.
        bool SubscriberPortBase::ConnectToPublihser(const std::string &pubEndpoint) {
            int rc = zsock_connect(m_port_socket, "%s", pubEndpoint.c_str());

            if (rc != 0) {
                std::cout << "Subscriber '" + GetPortBaseConfig()->portName + "' couldn't connect to " + pubEndpoint
                          << std::endl;
                return false;
            }

            m_logger->debug("Subscriber connected to: {} [{}]", pubEndpoint, GetConfig()->messageType);
//            std::cout << "Subscriber connected to: " << pubEndpoint << std::endl;
            return true;
        }

        const component_port_sub* SubscriberPortBase::GetConfig() const{
            return (component_port_sub*)GetPortBaseConfig();
        }

        SubscriberPortBase::~SubscriberPortBase() {

        }
    }
}
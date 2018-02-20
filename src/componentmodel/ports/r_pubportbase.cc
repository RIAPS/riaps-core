//
// Created by istvan on 10/4/17.
//

#include <componentmodel/ports/r_pubportbase.h>
#include <framework/rfw_network_interfaces.h>

namespace riaps{
    namespace ports{

        PublisherPortBase::PublisherPortBase(const component_port_config* config, const ComponentBase* parentComponent)
                : PortBase(PortTypes::Publisher, config, parentComponent),
                  SenderPort(this) {

        }

        PublisherPortBase::~PublisherPortBase(){

        }

        void PublisherPortBase::InitSocket() {
            m_port_socket = zsock_new(ZMQ_PUB);

            if (GetConfig()->isLocal){
                m_host = "127.0.0.1";
            } else {
                m_host = riaps::framework::Network::GetIPAddress();
            }

            if (m_host == "") {
                throw std::runtime_error("Publisher cannot be initiated. Cannot find  available network interface.");
            }

            std::string pub_endpoint = "tcp://" + m_host + ":!";
            m_port = zsock_bind(m_port_socket, "%s", pub_endpoint.c_str());


            if (m_port == -1) {
                throw std::runtime_error("Couldn't bind publisher port.");
            }

            m_logger->debug("Publisher is created on {}:{} [{}]", m_host, m_port, GetConfig()->messageType);
        }

        const _component_port_pub* PublisherPortBase::GetConfig() const {
            return (_component_port_pub*)GetPortBaseConfig();
        }

        std::string PublisherPortBase::GetEndpoint() {
            if (m_port_socket) {
                return std::string(zsock_endpoint(m_port_socket));
            }
            return "";
        }

//        bool PublisherPortBase::Send(zmsg_t** zmessage) const {
//            //const _component_port_pub_j* currentConfig = GetConfig();
//            //std::string messageType = currentConfig->messageType;
//            //zmsg_pushstr(*zmessage, messageType.c_str());
//
//            int rc = zmsg_send(zmessage, _port_socket);
//            return rc == 0;
//        }
    }
}
//
// Created by istvan on 10/4/17.
//

#include <componentmodel/ports/r_pubportbase.h>
#include <framework/rfw_network_interfaces.h>

namespace riaps{
    namespace ports{

        PublisherPortBase::PublisherPortBase(const component_port_config* config)
                : PortBase(PortTypes::Publisher, config),
                  SenderPort(this) {

        }

        PublisherPortBase::~PublisherPortBase(){

        }

        void PublisherPortBase::InitSocket() {
            _port_socket = zsock_new(ZMQ_PUB);

            if (GetConfig()->isLocal){
                _host = "127.0.0.1";
            } else {
                _host = riaps::framework::Network::GetIPAddress();
            }

            if (_host == "") {
                throw std::runtime_error("Publisher cannot be initiated. Cannot find  available network interface.");
            }

            std::string pub_endpoint = "tcp://" + _host + ":!";
            _port = zsock_bind(_port_socket, pub_endpoint.c_str());


            if (_port == -1) {
                throw std::runtime_error("Couldn't bind publisher port.");
            }

            std::cout << "Publisher is created on : " << _host << ":" << _port << std::endl;
        }

        const _component_port_pub* PublisherPortBase::GetConfig() const {
            return (_component_port_pub*)GetPortBaseConfig();
        }

        std::string PublisherPortBase::GetEndpoint() {
            if (_port_socket) {
                return std::string(zsock_endpoint(_port_socket));
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
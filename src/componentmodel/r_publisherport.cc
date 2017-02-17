//
// Created by parallels on 9/6/16.
//

#include <componentmodel/r_publisherport.h>
#include <componentmodel/r_network_interfaces.h>

namespace riaps{

    namespace ports {

        PublisherPort::PublisherPort(const _component_port_pub_j &config, ComponentBase *parent_component)
            : PortBase(PortTypes::Publisher, (component_port_config*)&config)

        {
            _port_socket = zsock_new(ZMQ_PUB);

            _host = GetIPAddress();

            if (_host == "") {
                throw std::runtime_error("Publisher cannot be initiated. Cannot find  available network interface.");
            }

            std::string pub_endpoint = "tcp://" + _host + ":!";
            _port = zsock_bind(_port_socket, pub_endpoint.c_str());


            if (_port == -1) {
                throw std::runtime_error("Couldn't bind publisher port.");
            }

            std::cout << "Publisher is created on : " << _host << ":" << _port << std::endl;


            if (!register_service(parent_component->GetActor()->GetApplicationName(),
                                  config.messageType,
                                  _host,
                                  _port,
                                  Kind::PUB,
                                  (config.isLocal?Scope::LOCAL:Scope::GLOBAL),
                                  {})) {
                throw std::runtime_error("Publisher port couldn't be registered.");
            }

        }

        _component_port_pub_j* PublisherPort::GetConfig() {
            return (_component_port_pub_j*)_config;
        }

        std::string PublisherPort::GetEndpoint() {
            if (_port_socket) {
                return std::string(zsock_endpoint(_port_socket));
            }
            return "";
        }


        // Before sending the publisher sets up the message type
        void PublisherPort::Send(zmsg_t *msg) const {
            zmsg_pushstr(msg, ((_component_port_pub_j*)_config)->messageType.c_str());

            int rc = zmsg_send(&msg, _port_socket);
            assert(rc == 0);
        }

        PublisherPort::~PublisherPort() {
            std::cout << "Publisherport " << _config->portName << " is stopping" <<std::endl;
        }
    }
}
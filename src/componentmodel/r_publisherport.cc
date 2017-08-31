//
// Created by parallels on 9/6/16.
//

#include <componentmodel/r_publisherport.h>
#include <framework/rfw_network_interfaces.h>


namespace riaps{

    namespace ports {

        PublisherPort::PublisherPort(const _component_port_pub_j &config, ComponentBase *parent_component)
            : PortBase(PortTypes::Publisher, (component_port_config*)&config)

        {
            _port_socket = zsock_new(ZMQ_PUB);

            if (config.isLocal){
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


            if (!registerService(parent_component->GetActor()->GetApplicationName(),
                                  config.messageType,
                                  _host,
                                  _port,
                                  riaps::discovery::Kind::PUB,
                                  (config.isLocal?riaps::discovery::Scope::LOCAL:riaps::discovery::Scope::GLOBAL),
                                  {})) {
                throw std::runtime_error("Publisher port couldn't be registered.");
            }

        }

        const _component_port_pub_j* PublisherPort::GetConfig() const {
            return (_component_port_pub_j*)GetPortBaseConfig();
        }

        std::string PublisherPort::GetEndpoint() {
            if (_port_socket) {
                return std::string(zsock_endpoint(_port_socket));
            }
            return "";
        }

        PublisherPort* PublisherPort::AsPublishPort() {
            return this;
        }

        bool PublisherPort::Send(zmsg_t** zmessage) const {
            //const _component_port_pub_j* currentConfig = GetConfig();
            //std::string messageType = currentConfig->messageType;
            //zmsg_pushstr(*zmessage, messageType.c_str());

            int rc = zmsg_send(zmessage, _port_socket);
            return rc == 0;
        }

//        bool PublisherPort::Send(std::string& message) const{
//            zmsg_t* zmsg = zmsg_new();
//            zmsg_addstr(zmsg, message.c_str());
//
//            return Send(&zmsg);
//        }
//
//        bool PublisherPort::Send(std::vector<std::string>& fields) const {
//            zmsg_t* zmsg = zmsg_new();
//
//            for (auto it = fields.begin(); it!=fields.end(); it++){
//                zmsg_addstr(zmsg, it->c_str());
//            }
//
//            return Send(&zmsg);
//        }

        PublisherPort::~PublisherPort() {
            // std::cout << "Publisherport " << GetConfig()->portName << " is stopping" <<std::endl;
        }
    }
}
//
// Created by parallels on 9/6/16.
//

#include <componentmodel/r_subscriberport.h>

namespace riaps{

    namespace ports {


        SubscriberPort::SubscriberPort(const _component_port_sub &config, const ComponentBase *component)
                : PortBase(PortTypes::Subscriber, (component_port_config*)(&config)),
                  _parent_component(component) {

            _port_socket = zsock_new(ZMQ_SUB);
            assert(_port_socket);
            zsock_set_subscribe(_port_socket, "");
        }


        /// \param pub_endpoint The endpoint, INCLUDING the transport layer. e.g.: tcp://192.168.1.1:4245
        /// \return True if the connection successful. False otherwise.
        bool SubscriberPort::ConnectToPublihser(const std::string &pub_endpoint) {
            int rc = zsock_connect(_port_socket, pub_endpoint.c_str());

            if (rc != 0) {
                std::cout << "Subscriber '" + GetPortBaseConfig()->portName + "' couldn't connect to " + pub_endpoint
                          << std::endl;
                return false;
            }

            std::cout << "Subscriber connected to: " << pub_endpoint << std::endl;
            return true;
        }

        void SubscriberPort::Init() {

            _component_port_sub* currentConfig = (_component_port_sub*)GetConfig();

            auto results =
                    subscribeToService(_parent_component->GetActor()->GetApplicationName(),
                                         _parent_component->GetConfig().component_name,
                                         _parent_component->GetActor()->GetActorName(),
                                         riaps::discovery::Kind::SUB,
                                         (currentConfig->isLocal?riaps::discovery::Scope::LOCAL:riaps::discovery::Scope::GLOBAL),
                                          currentConfig->portName, // Subscriber name
                                          currentConfig->messageType);

            for (auto result : results) {
                std::string endpoint = "tcp://" + result.host_name + ":" + std::to_string(result.port);
                ConnectToPublihser(endpoint);
            }
        }

        const _component_port_sub* SubscriberPort::GetConfig() const{
            return (_component_port_sub*)GetPortBaseConfig();
        }

        SubscriberPort* SubscriberPort::AsSubscribePort() {
            return this;
        }



        SubscriberPort::~SubscriberPort() {

        }
    }

}
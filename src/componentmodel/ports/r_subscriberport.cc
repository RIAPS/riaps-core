//
// Created by parallels on 9/6/16.
//

#include <componentmodel/ports/r_subscriberport.h>
#include <framework/rfw_network_interfaces.h>

namespace riaps{

    namespace ports {


        SubscriberPort::SubscriberPort(const component_port_sub &config, const ComponentBase *parentComponent)
                : SubscriberPortBase((component_port_config*)&config, parentComponent) {


        }




        void SubscriberPort::Init() {

            component_port_sub* current_config = (component_port_sub*)GetConfig();
            const std::string host = (current_config->isLocal) ? "127.0.0.1" : riaps::framework::Network::GetIPAddress();

            auto results =
                    subscribeToService(parent_component()->actor()->application_name(),
                                       parent_component()->component_config().component_name,
                                       parent_component()->actor()->actor_name(),
                                         host,
                                         riaps::discovery::Kind::SUB,
                                         (current_config->isLocal?riaps::discovery::Scope::LOCAL:riaps::discovery::Scope::GLOBAL),
                                       current_config->portName, // Subscriber name
                                       current_config->messageType);
            for (auto& result : results) {
                std::string endpoint = "tcp://" + result.host_name + ":" + std::to_string(result.port);
                ConnectToPublihser(endpoint);
            }
        }




        SubscriberPort* SubscriberPort::AsSubscribePort() {
            return this;
        }



        SubscriberPort::~SubscriberPort() {

        }
    }

}
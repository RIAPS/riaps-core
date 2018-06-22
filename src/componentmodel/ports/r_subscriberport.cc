//
// Created by parallels on 9/6/16.
//

#include <componentmodel/ports/r_subscriberport.h>

namespace riaps{

    namespace ports {


        SubscriberPort::SubscriberPort(const component_port_sub &config, const ComponentBase *parentComponent)
                : SubscriberPortBase((component_port_config*)&config, parentComponent) {


        }




        void SubscriberPort::Init() {

            component_port_sub* currentConfig = (component_port_sub*)GetConfig();

            auto results =
                    subscribeToService(parent_component()->actor()->application_name(),
                                       parent_component()->GetConfig().component_name,
                                       parent_component()->actor()->actor_name(),
                                         riaps::discovery::Kind::SUB,
                                         (currentConfig->isLocal?riaps::discovery::Scope::LOCAL:riaps::discovery::Scope::GLOBAL),
                                          currentConfig->portName, // Subscriber name
                                          currentConfig->messageType);
            m_logger->debug("After subscribe");
            for (auto result : results) {
                std::string endpoint = "tcp://" + result.host_name + ":" + std::to_string(result.port);
                ConnectToPublihser(endpoint);
            }
            m_logger->debug("{} end", __FUNCTION__);
        }




        SubscriberPort* SubscriberPort::AsSubscribePort() {
            return this;
        }



        SubscriberPort::~SubscriberPort() {

        }
    }

}
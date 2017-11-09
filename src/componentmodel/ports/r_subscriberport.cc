//
// Created by parallels on 9/6/16.
//

#include <componentmodel/ports/r_subscriberport.h>

namespace riaps{

    namespace ports {


        SubscriberPort::SubscriberPort(const _component_port_sub &config, const ComponentBase *component)
                : SubscriberPortBase((component_port_config*)&config), _parent_component(component) {


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



        SubscriberPort* SubscriberPort::AsSubscribePort() {
            return this;
        }



        SubscriberPort::~SubscriberPort() {

        }
    }

}
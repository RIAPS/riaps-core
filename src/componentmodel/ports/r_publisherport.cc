#include <componentmodel/ports/r_publisherport.h>
#include <framework/rfw_network_interfaces.h>
#include <componentmodel/r_discoverdapi.h>

namespace riaps{
    namespace ports {

        PublisherPort::PublisherPort(const component_port_pub &config, const ComponentBase* parent)
            : PublisherPortBase((component_port_config*)&config, parent)

        {
            InitSocket();
            if (!registerService(parent_component()->actor()->application_name(),
                                 parent_component()->actor()->actor_name(),
                                 config.message_type,
                                 host_,
                                 port_,
                                 riaps::discovery::Kind::PUB,
                                 (config.is_local?riaps::discovery::Scope::LOCAL:riaps::discovery::Scope::GLOBAL),
                                 {})) {
                throw std::runtime_error("Publisher port couldn't be registered.");
            }

        }

        PublisherPort* PublisherPort::AsPublishPort() {
            return this;
        }

        PublisherPort::~PublisherPort() {
        }
    }
}
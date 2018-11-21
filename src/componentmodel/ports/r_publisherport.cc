#include <componentmodel/ports/r_publisherport.h>
#include <framework/rfw_network_interfaces.h>
#include <componentmodel/r_discoverdapi.h>

namespace riaps{
    namespace ports {

        PublisherPort::PublisherPort(const ComponentPortPub &config, const ComponentBase* parent)
            : PublisherPortBase((ComponentPortConfig*)&config, parent)

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

                logger_->error("Publisher port couldn't be registered.");
            }
        }
    }
}
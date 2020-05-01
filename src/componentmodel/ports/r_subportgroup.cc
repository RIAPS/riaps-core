//
// Created by istvan on 10/17/17.
//

#include <componentmodel/ports/r_subportgroup.h>
#include <framework/rfw_network_interfaces.h>
#include <componentmodel/r_discoverdapi.h>

using namespace std;
using namespace riaps::discovery;

namespace riaps{
    namespace ports{

        GroupSubscriberPort::GroupSubscriberPort(const ComponentPortSub &config,
                                                 const ComponentBase* parentComponent,
                                                 const groups::GroupId& group_id)
            : group_port_config_(config),
              group_id_(group_id),
              SubscriberPortBase(&group_port_config_, parentComponent) {

        }

        void GroupSubscriberPort::Init() {
            const ComponentPortSub* current_config = (ComponentPortSub*)GetConfig();
            const string host = (current_config->is_local) ? "127.0.0.1" : riaps::framework::Network::GetIPAddress();

            const auto port_name = fmt::format("{}.{}",
                    group_id_.group_type_id,
                    group_id_.group_name);

            // Note: I've no idea why the python side is doing this.
            const auto message_type = fmt::format("{}@{}", current_config->message_type, port_name);

            auto results =
                    Disco::SubscribeToService(
                            parent_component()->actor()->application_name(),
                            parent_component()->component_config().component_name,
                            parent_component()->actor()->actor_name(),
                            host,
                            riaps::discovery::Kind::GSUB,
                            (current_config->is_local ? riaps::discovery::Scope::LOCAL
                                                      : riaps::discovery::Scope::GLOBAL),
                            port_name,
                            message_type);
            for (auto& result : results) {
                string endpoint = fmt::format("tcp://{}:{}", result.host_name, result.port);
                ConnectToPublihser(endpoint);
            }
        }
    }
}
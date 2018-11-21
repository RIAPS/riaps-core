#include <componentmodel/ports/r_pubportgroup.h>

namespace riaps{
    namespace ports{
        GroupPublisherPort::GroupPublisherPort(const GroupPortPub &config, const ComponentBase* parentComponent)
                : group_port_config_(config),
                  PublisherPortBase(&group_port_config_, parentComponent) {
            InitSocket();
        }

        riaps::groups::GroupService GroupPublisherPort::GetGroupService() {
            riaps::groups::GroupService result;

            result.address     = fmt::format("{}:{}",host_,port_);
            result.message_type = GetConfig()->message_type;

            return result;
        }
    }
}
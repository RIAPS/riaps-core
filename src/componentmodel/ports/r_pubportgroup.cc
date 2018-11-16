#include <componentmodel/ports/r_pubportgroup.h>

namespace riaps{
    namespace ports{
        GroupPublisherPort::GroupPublisherPort(const group_port_pub &config, const ComponentBase* parentComponent)
                : m_groupPortConfig(config),
                  PublisherPortBase(&m_groupPortConfig, parentComponent) {
            InitSocket();
        }

        riaps::groups::GroupService GroupPublisherPort::GetGroupService() {
            riaps::groups::GroupService result;

            result.address     = fmt::format("{}:{}",host_,port_);
            result.message_type = GetConfig()->message_type;

            return result;
        }

        GroupPublisherPort* GroupPublisherPort::AsGroupPublishPort() {
            return this;
        }

        GroupPublisherPort::~GroupPublisherPort() {

        }
    }
}
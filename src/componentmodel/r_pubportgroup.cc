#include <componentmodel/r_pubportgroup.h>

namespace riaps{
    namespace ports{
        GroupPublisherPort::GroupPublisherPort(const _group_port_pub &config)
                : _groupPortConfig(config), PublisherPortBase(&_groupPortConfig) {
            InitSocket();
        }

        riaps::groups::GroupService GroupPublisherPort::GetGroupService() {
            riaps::groups::GroupService result;

            result.address     = _host + ":" + std::to_string(_port);
            result.messageType = GetConfig()->messageType;

            return result;
        }

        GroupPublisherPort* GroupPublisherPort::AsGroupPublishPort() {
            return this;
        }

        GroupPublisherPort::~GroupPublisherPort() {

        }
    }
}
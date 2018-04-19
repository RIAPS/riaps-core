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

            result.address     = m_host + ":" + std::to_string(m_port);
            result.messageType = GetConfig()->messageType;

            return result;
        }

//        bool GroupPublisherPort::Send(capnp::MallocMessageBuilder &message) const {
//            zmsg_t* msg = nullptr;
//            msg << message;
//
//            // Add the componentId as the first frame of the message
//            zmsg_pushstr(msg, _componentId.c_str());
//
//            return Send(&msg);
//        }

        GroupPublisherPort* GroupPublisherPort::AsGroupPublishPort() {
            return this;
        }

        GroupPublisherPort::~GroupPublisherPort() {

        }
    }
}
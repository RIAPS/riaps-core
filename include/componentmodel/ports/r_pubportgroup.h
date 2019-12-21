//
// Created by istvan on 10/4/17.
//

#ifndef RIAPS_CORE_R_GPUBPORT_H
#define RIAPS_CORE_R_GPUBPORT_H


#include <groups/r_groupservice.h>
#include <componentmodel/r_configuration.h>
#include <componentmodel/ports/r_pubportbase.h>

namespace riaps{

    namespace groups{
        struct GroupService;
    }

    namespace ports{
        class GroupPublisherPort : public PublisherPortBase {
        public:
            GroupPublisherPort(const GroupPortPub& config, const ComponentBase* parentComponent);
            ::riaps::groups::GroupService GetGroupService();
            //bool SenderPort::Send(capnp::MallocMessageBuilder &message) const;
            ~GroupPublisherPort() = default;

        private:
            // The group configurations may dynamic (not from the model file)
            // GroupPorts store the original config.
            const ComponentPortPub group_port_config_;
            const std::string      component_id_;
        };
    }
}
#endif //RIAPS_CORE_R_GPUBPORT_H

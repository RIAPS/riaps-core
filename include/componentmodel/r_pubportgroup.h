//
// Created by istvan on 10/4/17.
//

#ifndef RIAPS_CORE_R_GPUBPORT_H
#define RIAPS_CORE_R_GPUBPORT_H


#include <groups/r_group.h>
#include <componentmodel/r_configuration.h>
#include <componentmodel/r_pubportbase.h>

namespace riaps{

    namespace groups{
        struct GroupService;
    }

    namespace ports{
        class GroupPublisherPort : public PublisherPortBase {
        public:
            GroupPublisherPort(const _group_port_pub& config);
            ::riaps::groups::GroupService GetGroupService();
            GroupPublisherPort* AsGroupPublishPort();
            ~GroupPublisherPort();

        private:
            // The group configurations may dynamic (not from the model file)
            // GroupPorts store the original config.
            const _component_port_pub _groupPortConfig;
        };
    }
}
#endif //RIAPS_CORE_R_GPUBPORT_H

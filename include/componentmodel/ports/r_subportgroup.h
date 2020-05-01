//
// Created by istvan on 10/17/17.
//

#ifndef RIAPS_CORE_R_SUBPORTGROUP_H
#define RIAPS_CORE_R_SUBPORTGROUP_H

#include <groups/r_groupid.h>
#include <componentmodel/ports/r_subportbase.h>

namespace riaps{
    namespace ports{
        class GroupSubscriberPort : public SubscriberPortBase {
        public:
            GroupSubscriberPort(const ComponentPortSub &config,
                                const ComponentBase* parentComponent,
                                const groups::GroupId& group_id);
            virtual void Init();
            virtual ~GroupSubscriberPort() = default;

        private:
            // The group configurations may dynamic (not from the model file)
            // GroupPorts store the original config.
            const ComponentPortSub group_port_config_;
            const groups::GroupId group_id_;
        };
    }
}

#endif //RIAPS_CORE_R_SUBPORTGROUP_H

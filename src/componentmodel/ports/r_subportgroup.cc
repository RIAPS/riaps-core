//
// Created by istvan on 10/17/17.
//

#include <componentmodel/ports/r_subportgroup.h>

namespace riaps{
    namespace ports{

        GroupSubscriberPort::GroupSubscriberPort(const component_port_sub &config, const ComponentBase* parentComponent)
            : group_port_config_(config), SubscriberPortBase(&group_port_config_, parentComponent) {
        }
    }
}
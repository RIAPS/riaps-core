//
// Created by istvan on 10/17/17.
//

#include <componentmodel/ports/r_subportgroup.h>

namespace riaps{
    namespace ports{

        GroupSubscriberPort::GroupSubscriberPort(const component_port_sub &config, const ComponentBase* parentComponent)
            : _groupPortConfig(config), SubscriberPortBase(&_groupPortConfig, parentComponent) {
        }

        GroupSubscriberPort* GroupSubscriberPort::AsGroupSubscriberPort() {
            return this;
        }

        GroupSubscriberPort::~GroupSubscriberPort() {

        }

    }
}
//
// Created by istvan on 10/17/17.
//

#include <componentmodel/r_subportgroup.h>

namespace riaps{
    namespace ports{

        GroupSubscriberPort::GroupSubscriberPort(const _component_port_sub &config)
            : SubscriberPortBase((component_port_config*)&config){

        }

        GroupSubscriberPort* GroupSubscriberPort::AsGroupSubscriberPort() {
            return this;
        }

        GroupSubscriberPort::~GroupSubscriberPort() {

        }

    }
}
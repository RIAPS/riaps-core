//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_R_SUBSCRIBERPORT_H
#define RIAPS_R_SUBSCRIBERPORT_H

#include <memory>
#include <componentmodel/r_componentbase.h>
#include <componentmodel/r_configuration.h>
#include <componentmodel/r_subportbase.h>

namespace riaps {

    class ComponentBase;

    namespace ports {
        class SubscriberPort : public SubscriberPortBase {
        public:

            SubscriberPort(const _component_port_sub &config, const ComponentBase *component);

            virtual void Init();

            virtual SubscriberPort* AsSubscribePort() ;

            ~SubscriberPort();

        protected:

            const ComponentBase *_parent_component;
        };
    }

}

#endif //RIAPS_R_SUBSCRIBERPORT_H

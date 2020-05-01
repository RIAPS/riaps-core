//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_R_SUBSCRIBERPORT_H
#define RIAPS_R_SUBSCRIBERPORT_H


#include <componentmodel/ports/r_subportbase.h>

namespace riaps {

    class ComponentBase;

    namespace ports {
        class SubscriberPort : public SubscriberPortBase {
        public:

            SubscriberPort(const ComponentPortSub &config, const ComponentBase *parentComponent);
            virtual void Init() override;
            virtual ~SubscriberPort() = default;
        };
    }

}

#endif //RIAPS_R_SUBSCRIBERPORT_H

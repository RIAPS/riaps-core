//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_R_SUBSCRIBERPORT_H
#define RIAPS_R_SUBSCRIBERPORT_H

#include "r_portbase.h"

namespace riaps {

    class SubscriberPort : public PortBase{
    public:
        SubscriberPort(subscriber_conf& config);



        virtual ~SubscriberPort();
    };

}

#endif //RIAPS_R_SUBSCRIBERPORT_H

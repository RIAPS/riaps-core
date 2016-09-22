//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_R_SUBSCRIBERPORT_H
#define RIAPS_R_SUBSCRIBERPORT_H

#include "r_portbase.h"
#include <memory>

namespace riaps {

    class SubscriberPort : public PortBase{
    public:

        static std::unique_ptr<SubscriberPort> InitFromServiceDetails(service_details& target_service);

        static void GetRemoteServiceAsync(subscriber_conf& config, std::string asyncendpoint);
        static service_details GetRemoteService(subscriber_conf& config);

        virtual ~SubscriberPort();
    protected:
        SubscriberPort();

    };

}

#endif //RIAPS_R_SUBSCRIBERPORT_H

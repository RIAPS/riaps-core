//
// Created by parallels on 9/29/16.
//

#ifndef RIAPS_FW_R_REQUESTPORT_H
#define RIAPS_FW_R_REQUESTPORT_H

#include "r_portbase.h"

namespace riaps {
    class RequestPort : public PortBase{
    public:

        static std::unique_ptr<RequestPort> InitFromServiceDetails(service_details& target_service);

        static void GetRemoteServiceAsync(request_conf& config, std::string asyncendpoint);
        static service_details GetRemoteService(request_conf& config);

        virtual ~RequestPort();
    protected:
        RequestPort();

    };
}

#endif //RIAPS_FW_R_REQUESTPORT_H

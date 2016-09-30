//
// Created by parallels on 9/29/16.
//

#ifndef RIAPS_FW_R_REQUESTPORT_H
#define RIAPS_FW_R_REQUESTPORT_H

#include "r_portbase.h"

namespace riaps {
    class RequestPort : public PortBase{
    public:

        RequestPort(request_conf& config);

        //static std::unique_ptr<RequestPort> InitFromConfig(request_conf& target_service);



        zmsg_t* SendMessage(zmsg_t** msg);

        //static std::unique_ptr<RequestPort> InitFromServiceDetails(service_details& target_service);

        //static void GetRemoteServiceAsync(request_conf& config, std::string asyncendpoint);

        virtual ~RequestPort();
    protected:
        service_details GetRemoteService(request_conf& config);
        request_conf _config;

    };
}

#endif //RIAPS_FW_R_REQUESTPORT_H

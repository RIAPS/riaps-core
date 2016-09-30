//
// Created by parallels on 9/29/16.
//

#ifndef RIAPS_FW_R_RESPONSEPORT_H
#define RIAPS_FW_R_RESPONSEPORT_H

#include "r_portbase.h"

namespace riaps {
    class ResponsePort : public PortBase {
    public:

        static std::unique_ptr<ResponsePort> InitFromConfig(response_conf& response_service);

        std::string GetEndpoint();

        response_conf GetConfig();

        void SendMessage(zmsg_t** msg);

        virtual ~ResponsePort();

    protected:
        ResponsePort();
        //ResponsePort(response_conf& config);

        response_conf configuration;

        int            port;
        std::string    endpoint;
    };
}

#endif //RIAPS_FW_R_RESPONSEPORT_H

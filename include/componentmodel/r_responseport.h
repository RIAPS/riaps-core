//
// Created by parallels on 9/29/16.
//

#ifndef RIAPS_FW_R_RESPONSEPORT_H
#define RIAPS_FW_R_RESPONSEPORT_H

#include "r_portbase.h"

namespace riaps {
    class ResponsePort : public PortBase {
    public:
        ResponsePort(response_conf& config);

        std::string GetEndpoint();

        publisher_conf GetConfig();

        void SendMessage(zmsg_t** msg);

        virtual ~ResponsePort();

    protected:
        response_conf configuration;

        int            port;
        std::string    endpoint;
    };
}

#endif //RIAPS_FW_R_RESPONSEPORT_H

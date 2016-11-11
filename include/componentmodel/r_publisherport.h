//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_R_PUBLISHERPORT_H
#define RIAPS_R_PUBLISHERPORT_H

#include "czmq.h"
#include "r_portbase.h"

namespace riaps {
    class PublisherPort : public PortBase {
    public:
        PublisherPort(_component_port_pub_j& config, std::string app_name);

        std::string GetEndpoint();

        _component_port_pub_j GetConfig();

        void PublishMessage(zmsg_t** msg);

        virtual ~PublisherPort();

    protected:
        _component_port_pub_j _configuration;

        int                   _port;
        std::string           _endpoint;
    };
}

#endif //RIAPS_R_PUBLISHERPORT_H

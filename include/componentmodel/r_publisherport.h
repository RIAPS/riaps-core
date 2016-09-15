//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_R_PUBLISHERPORT_H
#define RIAPS_R_PUBLISHERPORT_H

#include <czmq.h>
#include "r_portbase.h"

namespace riaps {
    class PublisherPort : public PortBase {
    public:
        PublisherPort(publisher_conf& config);

        std::string GetEndpoint();

        publisher_conf GetConfig();

        void PublishMessage(zmsg_t** msg);

        virtual ~PublisherPort();

    protected:
        publisher_conf configuration;

        int            port;
        std::string    endpoint;
    };
}

#endif //RIAPS_R_PUBLISHERPORT_H

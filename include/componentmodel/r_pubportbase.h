//
// Created by istvan on 10/4/17.
//

#ifndef RIAPS_CORE_R_PUBLISHERPORTBASE_H
#define RIAPS_CORE_R_PUBLISHERPORTBASE_H

#include <componentmodel/r_portbase.h>

namespace riaps{
    namespace ports{
        class PublisherPortBase : public PortBase{
        public:
            PublisherPortBase(component_port_config* config);
            std::string GetEndpoint();

            virtual const _component_port_pub* GetConfig() const;
            virtual ~PublisherPortBase();

        protected:
            int _port;
            std::string _host;
            std::string _endpoint;

            virtual bool Send(zmsg_t** zmessage) const;
            virtual void InitSocket();
        };
    }
}

#endif //RIAPS_CORE_R_PUBLISHERPORTBASE_H

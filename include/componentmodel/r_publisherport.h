//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_R_PUBLISHERPORT_H
#define RIAPS_R_PUBLISHERPORT_H


#include "r_configuration.h"
#include "r_portbase.h"

#include "czmq.h"

#include <string>

namespace riaps {

    class ComponentBase;

    namespace ports {

       class PublisherPort : public PortBase {
       public:
           using PortBase::Send;

           PublisherPort(const _component_port_pub &config, ComponentBase *parent_component);

           std::string GetEndpoint();

           virtual const _component_port_pub* GetConfig() const;


           //virtual bool Send(std::string& message) const;
           //virtual bool Send(std::vector<std::string>& fields) const;

           virtual PublisherPort*  AsPublishPort() ;

           ~PublisherPort();

       protected:
           int _port;
           std::string _host;
           std::string _endpoint;

           virtual bool Send(zmsg_t** zmessage) const;
       };
   }
}

#endif //RIAPS_R_PUBLISHERPORT_H

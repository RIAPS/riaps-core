//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_R_PUBLISHERPORT_H
#define RIAPS_R_PUBLISHERPORT_H


#include <componentmodel/r_configuration.h>
#include <componentmodel/r_pubportbase.h>

#include <czmq.h>

#include <string>

namespace riaps {

    class ComponentBase;

    namespace ports {

       class PublisherPort : public PublisherPortBase {
       public:
           //using PortBase::Send;

           /**
            * Component publisher port, the port registers itself in the discovery service
            * @param config
            * @param parent_component
            */
           PublisherPort(const _component_port_pub& config);

           virtual PublisherPort*  AsPublishPort() ;

           ~PublisherPort();

       };
   }
}

#endif //RIAPS_R_PUBLISHERPORT_H

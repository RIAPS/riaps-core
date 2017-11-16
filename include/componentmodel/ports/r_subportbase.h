//
// Created by istvan on 10/13/17.
//

#ifndef RIAPS_CORE_R_SUBPORTBASE_H
#define RIAPS_CORE_R_SUBPORTBASE_H

#include <componentmodel/ports/r_portbase.h>

#include <string>

namespace riaps{
    namespace ports{
        class SubscriberPortBase : public PortBase{
        public:
            SubscriberPortBase(const component_port_config* config, const ComponentBase* parentComponent);

            virtual const _component_port_sub* GetConfig() const;

            // Returns false, if the subscriber couldn't connect
            bool ConnectToPublihser(const std::string& pub_endpoint);

            virtual ~SubscriberPortBase();

        protected:
            std::vector<std::string> _endpoints;

            virtual void InitSocket();
        };
    }
}

#endif //RIAPS_CORE_R_SUBPORTBASE_H

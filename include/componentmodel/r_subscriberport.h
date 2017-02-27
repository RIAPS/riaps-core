//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_R_SUBSCRIBERPORT_H
#define RIAPS_R_SUBSCRIBERPORT_H

#include <memory>
#include "r_componentbase.h"
#include "r_configuration.h"
#include "r_portbase.h"

namespace riaps {

    class ComponentBase;

    namespace ports {
        class SubscriberPort : public PortBase {
        public:

            SubscriberPort(const _component_port_sub_j &config, const ComponentBase *component);

            virtual void Init();

            // Returns false, if the subscriber couldn't connect
            bool ConnectToPublihser(const std::string& pub_endpoint);

            virtual SubscriberPort* AsSubscribePort() ;

            virtual const _component_port_sub_j* GetConfig() const;

            //static std::unique_ptr<SubscriberPort> InitFromServiceDetails(service_details& target_service);

            //static void GetRemoteServiceAsync(subscriber_conf& config, std::string asyncendpoint);
            //static service_details GetRemoteService(subscriber_conf& config);

            ~SubscriberPort();

        protected:

            const ComponentBase *_parent_component;

        //private:

            //virtual bool Send(zmsg_t** zmessage) const;


        };
    }

}

#endif //RIAPS_R_SUBSCRIBERPORT_H

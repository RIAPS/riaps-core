//
// Created by parallels on 9/29/16.
//

#ifndef RIAPS_FW_R_REQUESTPORT_H
#define RIAPS_FW_R_REQUESTPORT_H

#include "r_componentbase.h"
#include "r_configuration.h"

#include "czmq.h"

#include <string>

namespace riaps {

    class ComponentBase;

    namespace ports {
        class RequestPort : public PortBase {
        public:

            RequestPort(const _component_port_req_j &config, const ComponentBase *component);
            virtual void Init();

            //static std::unique_ptr<RequestPort> InitFromConfig(request_conf& target_service);

            // Returns false, if the request port couldn't connect
            bool ConnectToResponse(const std::string& rep_endpoint);

            virtual void Send(zmsg_t** msg) const;
            virtual void Send(std::string message) const;

            virtual std::string Recv();

            virtual RequestPort* AsRequestPort();

            // zmsg_t* SendMessage(zmsg_t** msg);

            //static std::unique_ptr<RequestPort> InitFromServiceDetails(service_details& target_service);

            //static void GetRemoteServiceAsync(request_conf& config, std::string asyncendpoint);

            //virtual ~RequestPort();
        protected:
            const ComponentBase *_parent_component;

        };
    }
}

#endif //RIAPS_FW_R_REQUESTPORT_H

//
// Created by parallels on 9/29/16.
//

#ifndef RIAPS_FW_R_REQUESTPORT_H
#define RIAPS_FW_R_REQUESTPORT_H

#include "r_componentbase.h"
#include "r_configuration.h"
#include "r_messagebase.h"

#include "czmq.h"

#include <string>

namespace riaps {

    class ComponentBase;

    namespace ports {
        class RequestPort : public PortBase {
        public:
            using PortBase::Send;

            RequestPort(const _component_port_req_j &config, const ComponentBase *component);
            virtual void Init();

            //static std::unique_ptr<RequestPort> InitFromConfig(request_conf& target_service);

            // Returns false, if the request port couldn't connect
            bool ConnectToResponse(const std::string& rep_endpoint);


            //virtual bool Send(std::string& message) const;
            //virtual bool Send(std::vector<std::string>& fields) const;

            virtual bool Recv(std::string& messageType, riaps::MessageBase* message);

            virtual bool Recv(std::string& messageType, capnp::FlatArrayMessageReader** messageReader);

            virtual RequestPort* AsRequestPort() ;

            virtual const _component_port_req_j* GetConfig() const;



            // zmsg_t* SendMessage(zmsg_t** msg);

            //static std::unique_ptr<RequestPort> InitFromServiceDetails(service_details& target_service);

            //static void GetRemoteServiceAsync(request_conf& config, std::string asyncendpoint);

            //virtual ~RequestPort();
        protected:
            const ComponentBase *_parent_component;
            bool _isConnected;

            capnp::FlatArrayMessageReader _capnpReader;

            virtual bool Send(zmsg_t** zmessage) const;

        };
    }
}

#endif //RIAPS_FW_R_REQUESTPORT_H

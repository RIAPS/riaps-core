//
// Created by parallels on 9/29/16.
//

#ifndef RIAPS_FW_R_REQUESTPORT_H
#define RIAPS_FW_R_REQUESTPORT_H

#include <componentmodel/r_componentbase.h>
#include <componentmodel/r_configuration.h>
#include <componentmodel/r_messagebase.h>
#include <componentmodel/ports/r_senderport.h>

#include <czmq.h>

#include <string>

namespace riaps {

    class ComponentBase;

    namespace ports {
        class AsyncRequestPort : public PortBase, public SenderPort {
        public:
            //using PortBase::Send;

            AsyncRequestPort(const _component_port_req &config, const ComponentBase *component);
            virtual void Init();

            // Returns false, if the request port couldn't connect
            bool ConnectToResponse(const std::string& rep_endpoint);

            virtual bool Recv(capnp::FlatArrayMessageReader** messageReader);

            virtual AsyncRequestPort* AsAsyncRequestPort() ;

            virtual const _component_port_req* GetConfig() const;

            ~AsyncRequestPort() noexcept ;
        protected:
            const ComponentBase *_parent_component;
            bool _isConnected;

            capnp::FlatArrayMessageReader _capnpReader;

            virtual bool Send(capnp::MallocMessageBuilder& message) const;

            zuuid_t* _socketId;

            //virtual bool Send(zmsg_t** zmessage) const;

        };
    }
}

#endif //RIAPS_FW_R_REQUESTPORT_H

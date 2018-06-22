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
        class RequestPort : public PortBase, public SenderPort, public RecvPort {
        public:
            //using PortBase::Send;

            RequestPort(const component_port_req &config, const ComponentBase *parentComponent);
            virtual void Init();

            // Returns false, if the request port couldn't connect
            bool ConnectToResponse(const std::string& rep_endpoint);

            virtual bool Recv(capnp::FlatArrayMessageReader** messageReader);

            virtual RequestPort* AsRequestPort();
            virtual RecvPort*    AsRecvPort()   ;


            virtual const component_port_req* GetConfig() const;

            const timespec& GetRecvTimestamp() const;



            ~RequestPort() noexcept ;
        protected:
            bool is_connected_;

            timespec m_recvTimestamp;

            capnp::FlatArrayMessageReader capnp_reader_;

            virtual bool Send(capnp::MallocMessageBuilder& message) const;
        };
    }
}

#endif //RIAPS_FW_R_REQUESTPORT_H

#ifndef RIAPS_FW_R_REQUESTPORT_H
#define RIAPS_FW_R_REQUESTPORT_H

#include <componentmodel/r_componentbase.h>
#include <componentmodel/r_configuration.h>
#include <componentmodel/ports/r_senderport.h>

#include <czmq.h>

#include <string>

namespace riaps {

    class ComponentBase;

    namespace ports {
        class RequestPort : public PortBase, public SenderPort, public RecvPort {
        public:

            RequestPort(const ComponentPortReq &config, const ComponentBase *parent);
            virtual void Init();

            // Returns false, if the request port couldn't connect
            bool ConnectToResponse(const std::string& rep_endpoint);

            //virtual bool Recv(capnp::FlatArrayMessageReader** messageReader);
            virtual const ComponentPortReq* GetConfig() const;
            const timespec& recv_timestamp() const;

            ~RequestPort() noexcept ;
        protected:
            bool is_connected_;
            timespec recv_timestamp_;
            capnp::FlatArrayMessageReader capnp_reader_;
            virtual bool Send(capnp::MallocMessageBuilder& message) const;
        };
    }
}

#endif //RIAPS_FW_R_REQUESTPORT_H

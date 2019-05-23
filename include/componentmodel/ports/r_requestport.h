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

            /**
             * Connects to a response port
             * @param rep_endpoint Address of the response port, where request wants to connect to.
             * @return Returns false, if the connection is unsuccessful.
             */
            bool ConnectToResponse(const std::string& rep_endpoint);

            /**
             * @return Configuration parameters of the request port.
             */
            virtual const ComponentPortReq* GetConfig() const;

            ~RequestPort() noexcept ;
        protected:
            bool is_connected_;
            timespec recv_timestamp_;
            capnp::FlatArrayMessageReader capnp_reader_;
            virtual PortError Send(capnp::MallocMessageBuilder& message) const override;
        };
    }
}

#endif //RIAPS_FW_R_REQUESTPORT_H

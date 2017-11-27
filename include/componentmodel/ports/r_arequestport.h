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
#include <zuuid.h>

#include <string>

namespace riaps {

    class ComponentBase;

    namespace ports {
        class AsyncRequestPort : public PortBase {
        public:

            AsyncRequestPort(const _component_port_req &config, const ComponentBase *component);
            virtual void Init();

            // Returns false, if the request port couldn't connect
            bool ConnectToResponse(const std::string& rep_endpoint);

            virtual bool Recv(capnp::FlatArrayMessageReader** messageReader);

            virtual AsyncRequestPort* AsAsyncRequestPort() ;

            virtual const _component_port_req* GetConfig() const;

            ~AsyncRequestPort() noexcept ;
        protected:
            bool _isConnected;

            capnp::FlatArrayMessageReader _capnpReader;

            /**
             * Converts the passed capnp message to bytes and sends the bytearray.
             *
             * The message frame is extended:
             *
             * *----------*
             * | FRAME 0  | -> socket id (automatically set by ZMQ)
             * +----------+
             * | FRAME 1  | -> createTime (timestamp, when the message is sent)
             * +----------+
             * | FRAME 2  | -> expirationTime (relative, calculated from the createTime, where the app expects responses)
             * +----------+
             * | FRAME 3  | -> requestId  (ZMQ generated unique id, zuuid_t struct represented as string)
             * +----------+
             * | FRAME 4  | -> the message param, converted to bytes
             * *----------*
             *
             *
             * @param message The message to be sent.
             * @param expiration Expiration time in msec. If greater than 0, then the message is recevied only if
             * (creatTime + expirationTime)<=currentTime. If 0, then no response is accepted. If -1, all response is
             * accepted in the future. Default value is -1, no expiration time is set.
             *
             * @return Request id. Response also contains it. Empty string if the Send() failed.
             *
             */

            // Note: extra parameter for expiration?
            const std::string Send(capnp::MallocMessageBuilder& message, int64_t expiration = -1) const;

            zuuid_t* _socketId;

            //virtual bool Send(zmsg_t** zmessage) const;

        };
    }
}

#endif //RIAPS_FW_R_REQUESTPORT_H

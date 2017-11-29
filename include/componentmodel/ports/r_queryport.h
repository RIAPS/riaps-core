//
// Created by parallels on 9/29/16.
//

#ifndef RIAPS_FW_R_QUERYPORT_H
#define RIAPS_FW_R_QUERYPORT_H

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
        class QueryPort : public PortBase {
        public:

            QueryPort(const _component_port_qry &config, const ComponentBase *component);
            virtual void Init();

            // Returns false, if the request port couldn't connect
            bool ConnectToResponse(const std::string& ansEndpoint);

            virtual bool Recv(capnp::FlatArrayMessageReader** messageReader);

            virtual QueryPort* AsQueryPort() ;

            virtual const _component_port_qry* GetConfig() const;

            ~QueryPort() noexcept ;
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
             * | FRAME 1  | -> requestId  (ZMQ generated unique id, zuuid_t struct represented as string)
             * +----------+
             * | FRAME 2  | -> the message to be sent, converted to bytes
             * +----------+
             * | FRAME 3  | -> timestamp (automatically added by riaps, if addTimestamp parameter is true)
             * +----------+
             *
             * @param message The message to be sent.
             * @param addTimestamp if true, timestamp is added as payload. using the system clock (NOT the mono clock)
             * @return Request id. Response also contains it. Empty string if the Send() failed.
             *
             */

            const std::string Send(capnp::MallocMessageBuilder& message, bool addTimestamp = false) const;

            zuuid_t* _socketId;

        };
    }
}

#endif //RIAPS_FW_R_QUERYPORT_H

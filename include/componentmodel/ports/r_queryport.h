#ifndef RIAPS_FW_R_QUERYPORT_H
#define RIAPS_FW_R_QUERYPORT_H

#include <componentmodel/r_componentbase.h>
#include <componentmodel/r_configuration.h>

#include <czmq.h>
#include <zuuid.h>

#include <string>

namespace riaps {

    class ComponentBase;

    namespace ports {
        class QueryPort;
        class GroupQueryPort;



        class QueryPort : public PortBase {
        public:

            /**
             * @param config Configuration. Comes from the riaps file.
             * @param component The parent component.
             */
            QueryPort(const ComponentPortQry &config, const ComponentBase *component);
            void Init();

            /**
             * Connects to a server.
             * @param ans_endpoint The address of the server. (Answer type port)
             * @return False, if the request port couldn't connect. True otherwise.
             */
            bool Connect(const std::string& ans_endpoint);
            void Disconnect(const std::string& ans_endpoint);

//            template<class R, class T>
//            bool RecvQuery(std::shared_ptr<riaps::RiapsMessage<R, T>>& message,
//                           std::shared_ptr<riaps::MessageParams>& params);

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
             * | FRAME 3  | -> timestamp (added by the framework, if addTimestamp parameter is true)
             * +----------+
             *
             * @param message The message to be sent.
             * @param addTimestamp if true, timestamp is added as payload. using the system clock (NOT the mono clock)
             * @return Request id. Response also contains it. Empty string if the Send() failed.
             *
             */

            PortError SendQuery(capnp::MallocMessageBuilder& message, std::string& requestId, bool addTimestamp = false) const;
            virtual const ComponentPortQry* GetConfig() const;

            ~QueryPort() noexcept ;
        protected:
            bool connected_;
            capnp::FlatArrayMessageReader capnp_reader_;
            zuuid_t* socketid_;
        };

//        template<class R, class T>
//        bool QueryPort::RecvQuery(std::shared_ptr<riaps::RiapsMessage<R, T>>& message,
//                       std::shared_ptr<riaps::MessageParams>& params){
//            /**
//             * |RequestId|Message|Timestamp|
//            */
//
//            char* cRequestId = nullptr;
//            zframe_t *bodyFrame = nullptr, *timestampFrame = nullptr;
//            if (zsock_recv(port_socket_, "sff", &cRequestId, &bodyFrame, &timestampFrame)==0){
//                std::string socketId = zuuid_str(socketid_);
//                params.reset(new riaps::MessageParams(socketId, &cRequestId, &timestampFrame));
//                message.reset(new RiapsMessage<R, T>(&bodyFrame));
//            } else {
//                logger()->error("Wrong incoming message format on port: {}", port_name());
//            }
//
//            return false;
//        };
    }
}

#endif //RIAPS_FW_R_QUERYPORT_H

//
// Created by istvan on 9/30/16.
//

#include <componentmodel/ports/r_queryport.h>
#include <fmt/format.h>
#include <framework/rfw_network_interfaces.h>

namespace riaps {
    namespace ports {

        QueryPort::QueryPort(const component_port_qry &config, const ComponentBase *component)
                : PortBase(PortTypes::Query,
                           (component_port_config*)(&config),
                           component),
                  m_capnpReader(capnp::FlatArrayMessageReader(nullptr)) {
            port_socket_ = zsock_new(ZMQ_DEALER);
            m_socketId = zuuid_new();

            //auto i = zsock_rcvtimeo (_port_socket);

            int timeout = 500;//msec
            int lingerValue = 0;
            int connectTimeout = 1000; //msec
            zmq_setsockopt(port_socket_, ZMQ_SNDTIMEO, &timeout , sizeof(int));
            zmq_setsockopt(port_socket_, ZMQ_LINGER, &lingerValue, sizeof(int));
            zsock_set_identity(port_socket_, zuuid_str(m_socketId));

            //i = zsock_rcvtimeo (_port_socket);

            m_isConnected = false;
        }



        void QueryPort::Init() {

            const component_port_qry* current_config = GetConfig();
            const std::string host = (current_config->isLocal) ? "127.0.0.1" : riaps::framework::Network::GetIPAddress();

            auto results =
                    subscribeToService(parent_component()->actor()->application_name(),
                                       parent_component()->GetConfig().component_name,
                                       parent_component()->actor()->actor_name(),
                                       host,
                                       riaps::discovery::Kind::QRY,
                                       (current_config->isLocal?riaps::discovery::Scope::LOCAL:riaps::discovery::Scope::GLOBAL),
                                       current_config->portName, // Subscriber name
                                       current_config->messageType);

            for (auto result : results) {
                std::string endpoint = fmt::format("tcp://{0}:{1}", result.host_name, result.port);
                //std::string endpoint = "tcp://" + result.host_name + ":" + std::to_string(result.port);
                ConnectToResponse(endpoint);
            }
        }

        bool QueryPort::ConnectToResponse(const std::string &ansEndpoint) {
            int rc = zsock_connect(port_socket_, "%s", ansEndpoint.c_str());

            if (rc != 0) {
                m_logger->error("Queryport {} couldn't connect to {}", GetConfig()->portName, ansEndpoint);
                return false;
            }

            m_isConnected = true;
            m_logger->info("Queryport connected to: {}", ansEndpoint);
            return true;
        }

        const component_port_qry* QueryPort::GetConfig() const{
            return (component_port_qry*)GetPortBaseConfig();
        }

        QueryPort* QueryPort::AsQueryPort() {
            return this;
        }

//        bool QueryPort::RecvQuery(std::shared_ptr<capnp::FlatArrayMessageReader>& messageReader,
//                                  std::shared_ptr<riaps::MessageParams>& params) {
//            /**
//             * |RequestId|Message|Timestamp|
//             */
//
//            char* cRequestId = nullptr;
//            zframe_t *bodyFrame = nullptr, *timestampFrame = nullptr;
//            if (zsock_recv(_port_socket, "sff", &cRequestId, &bodyFrame, &timestampFrame)==0){
//                std::string socketId = zuuid_str(_socketId);
//                params.reset(new riaps::MessageParams(socketId, &cRequestId, &timestampFrame));
//            } else {
//                m_logger->error("Wrong incoming message format on port: {}", GetPortName());
//            }
//
//            return false;
//        }


        bool QueryPort::SendQuery(capnp::MallocMessageBuilder &message,std::string& requestId, bool addTimestamp) const {
            if (port_socket_ == nullptr || !m_isConnected){
                return false;
            }

            zframe_t* userFrame;
            userFrame << message;

            zframe_t* tsFrame = nullptr;
            if (addTimestamp){
                int64_t ztimeStamp = zclock_time();
                tsFrame = zframe_new(&ztimeStamp, sizeof(ztimeStamp));
            } else{
                tsFrame = zframe_new_empty();
            }


            // Create the timestamp
            //capnp::MallocMessageBuilder tsBuilder;
            //auto msgTimestamp = tsBuilder.initRoot<riaps::distrcoord::RiapsTimestamp>();

            // Build the timestamp
            //msgTimestamp.setValue(zclock_mono());
            //zmsg_t* zmsgTimestamp;
            //zmsgTimestamp << tsBuilder;

            // Create expiration time
            //capnp::MallocMessageBuilder expBuilder;
            //auto msgExpire = expBuilder.initRoot<riaps::distrcoord::RiapsTimestamp>();
            //msgExpire.setValue(expiration);
            //zmsg_t* zmsgExpiration;
            //zmsgExpiration << expBuilder;

            // Generate uniqueId
            std::string msgId;
            {
                auto id = zuuid_new();
                msgId.assign(zuuid_str(id));
                zuuid_destroy(&id);
            }

            int rc = zsock_send(const_cast<zsock_t*>(GetSocket()),
                                "sff",
                                msgId.c_str() ,
                                userFrame,
                                tsFrame
                                )
            ;

            zframe_destroy(&userFrame);
            zframe_destroy(&tsFrame);
            if (rc == 0) {
                requestId = msgId;
                return true;
            }
            return false;
        }

        QueryPort::~QueryPort() noexcept {
            zuuid_destroy(&m_socketId);
        }


    }
}

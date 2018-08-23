//
// Created by istvan on 9/30/16.
//

#include <componentmodel/ports/r_requestport.h>
#include <framework/rfw_network_interfaces.h>


namespace riaps {
    namespace ports {

        RequestPort::RequestPort(const component_port_req &config, const ComponentBase *parentComponent)
                : PortBase(PortTypes::Request, (component_port_config*)(&config), parentComponent),
                  SenderPort(this),
                  RecvPort(this),
                  capnp_reader_(capnp::FlatArrayMessageReader(nullptr)) {
            port_socket_ = zsock_new(ZMQ_REQ);
            int timeout = 500;//msec
            int lingerValue = 0;
            int connectTimeout = 1000; //msec
            zmq_setsockopt(port_socket_, ZMQ_SNDTIMEO, &timeout , sizeof(int));
            zmq_setsockopt(port_socket_, ZMQ_LINGER, &lingerValue, sizeof(int));

            is_connected_ = false;
        }



        void RequestPort::Init() {

            const component_port_req* current_config = GetConfig();
            const std::string host = (current_config->isLocal) ? "127.0.0.1" : riaps::framework::Network::GetIPAddress();

            auto results =
                    subscribeToService(parent_component()->actor()->application_name(),
                                       parent_component()->config().component_name,
                                       parent_component()->actor()->actor_name(),
                                         host,
                                         riaps::discovery::Kind::REQ,
                                         (current_config->isLocal?riaps::discovery::Scope::LOCAL:riaps::discovery::Scope::GLOBAL),
                                         current_config->portName, // Subscriber name
                                         current_config->messageType);

            for (auto& result : results) {
                std::string endpoint = "tcp://" + result.host_name + ":" + std::to_string(result.port);
                ConnectToResponse(endpoint);
            }
        }

        bool RequestPort::ConnectToResponse(const std::string &rep_endpoint) {
            int rc = zsock_connect(port_socket_, "%s", rep_endpoint.c_str());

            if (rc != 0) {
                std::cout << "Request '" + GetConfig()->portName + "' couldn't connect to " + rep_endpoint
                          << std::endl;
                return false;
            }

            is_connected_ = true;
            std::cout << "Request port connected to: " << rep_endpoint << std::endl;
            return true;
        }

        const component_port_req* RequestPort::GetConfig() const{
            return (component_port_req*)GetPortBaseConfig();
        }

        RequestPort* RequestPort::AsRequestPort() {
            return this;
        }

        const timespec& RequestPort::GetRecvTimestamp() const {
            return m_recvTimestamp;
        }

        // TODO: return shared_ptr instead of pointer
        bool RequestPort::Recv(capnp::FlatArrayMessageReader** messageReader) {
            auto capnpMessage = RecvPort::Recv();
            if (capnpMessage == nullptr) return false;


            *messageReader = capnpMessage.get();
            return true;

//            zmsg_t* msg = zmsg_recv((void*)GetSocket());
//
//            if (msg){
//                //char* msgType = zmsg_popstr(msg);
//                //message_type = msgType;
//                //if (msgType!=NULL){
//                zframe_t* bodyFrame = zmsg_pop(msg);
//                size_t size = zframe_size(bodyFrame);
//                byte* data = zframe_data(bodyFrame);
//
//                auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));
//                capnp_reader_ = capnp::FlatArrayMessageReader(capnp_data);
//                *messageReader = &capnp_reader_;
//
//                zframe_destroy(&bodyFrame);
//
//                if (GetConfig()->isTimed){
//                    auto timeFrame = zmsg_pop(msg);
//                    if (timeFrame) {
//                        auto buffer = zframe_data(timeFrame);
//                        double dTime;
//                        memcpy(&dTime, buffer, sizeof(double));
//                        m_recvTimestamp.tv_sec  = dTime;
//                        m_recvTimestamp.tv_nsec = (dTime-m_recvTimestamp.tv_sec)*BILLION;
//                        zframe_destroy(&timeFrame);
//                    }
//                }
//
//
//                return true;
//                //}
//                //return false;
//            }
//            zmsg_destroy(&msg);
//
//            return false;
        }

        RecvPort* RequestPort::AsRecvPort() {
            return this;
        }

        bool RequestPort::Send(capnp::MallocMessageBuilder &message) const {
            if (port_socket_ == nullptr || !is_connected_){
                return false;
            }

            return SenderPort::Send(message);

//            zmsg_t* zmsg;
//            zmsg << message;
//
//            int rc = zmsg_send(&zmsg, const_cast<zsock_t*>(GetSocket()));
//            return rc==0;
        }

        RequestPort::~RequestPort() noexcept {

        }
    }
}

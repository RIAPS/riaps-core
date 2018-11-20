#include <componentmodel/ports/r_requestport.h>
#include <framework/rfw_network_interfaces.h>

using namespace std;

namespace riaps {
    namespace ports {

        RequestPort::RequestPort(const component_port_req &config, const ComponentBase *parent)
                : PortBase(PortTypes::Request, (component_port_config*)(&config), parent),
                  SenderPort(this),
                  RecvPort(this),
                  capnp_reader_(capnp::FlatArrayMessageReader(nullptr)) {
            port_socket_ = zsock_new(ZMQ_REQ);
            int timeout = 500;//msec
            int linger = 0;
            int connect_timeout = 1000; //msec
            zmq_setsockopt(port_socket_, ZMQ_SNDTIMEO, &timeout , sizeof(int));
            zmq_setsockopt(port_socket_, ZMQ_LINGER, &linger, sizeof(int));

            is_connected_ = false;
        }

        void RequestPort::Init() {
            const component_port_req* current_config = GetConfig();
            const string host = (current_config->is_local) ? "127.0.0.1" : riaps::framework::Network::GetIPAddress();

            auto results =
                    subscribeToService(parent_component()->actor()->application_name(),
                                       parent_component()->component_config().component_name,
                                       parent_component()->actor()->actor_name(),
                                       host,
                                       riaps::discovery::Kind::REQ,
                                       (current_config->is_local?riaps::discovery::Scope::LOCAL:riaps::discovery::Scope::GLOBAL),
                                        current_config->port_name,
                                        current_config->message_type);

            for (auto& result : results) {
                string endpoint = fmt::format("tcp://{}:{}", result.host_name, result.port);
                ConnectToResponse(endpoint);
            }
        }

        bool RequestPort::ConnectToResponse(const std::string &rep_endpoint) {
            int rc = zsock_connect(port_socket_, "%s", rep_endpoint.c_str());

            // TODO: with spdlog
            if (rc != 0) {
                std::cout << "Request '" + GetConfig()->port_name + "' couldn't connect to " + rep_endpoint
                          << std::endl;
                return false;
            }

            is_connected_ = true;
            // TODO: spdlog
            std::cout << "Request port connected to: " << rep_endpoint << std::endl;
            return true;
        }

        const component_port_req* RequestPort::GetConfig() const{
            return (component_port_req*) config();
        }

//        RequestPort* RequestPort::AsRequestPort() {
//            return this;
//        }

        const timespec& RequestPort::recv_timestamp() const {
            return recv_timestamp_;
        }

        // TODO: return shared_ptr instead of pointer
//        bool RequestPort::Recv(capnp::FlatArrayMessageReader** messageReader) {
//            auto capnp_message = RecvPort::Recv();
//            if (capnp_message == nullptr) return false;
//
//            *messageReader = capnp_message.get();
//            return true;
//
////            zmsg_t* msg = zmsg_recv((void*)GetSocket());
////
////            if (msg){
////                //char* msgType = zmsg_popstr(msg);
////                //message_type = msgType;
////                //if (msgType!=NULL){
////                zframe_t* bodyFrame = zmsg_pop(msg);
////                size_t size = zframe_size(bodyFrame);
////                byte* data = zframe_data(bodyFrame);
////
////                auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));
////                capnp_reader_ = capnp::FlatArrayMessageReader(capnp_data);
////                *messageReader = &capnp_reader_;
////
////                zframe_destroy(&bodyFrame);
////
////                if (GetConfig()->isTimed){
////                    auto timeFrame = zmsg_pop(msg);
////                    if (timeFrame) {
////                        auto buffer = zframe_data(timeFrame);
////                        double dTime;
////                        memcpy(&dTime, buffer, sizeof(double));
////                        m_recvTimestamp.tv_sec  = dTime;
////                        m_recvTimestamp.tv_nsec = (dTime-m_recvTimestamp.tv_sec)*BILLION;
////                        zframe_destroy(&timeFrame);
////                    }
////                }
////
////
////                return true;
////                //}
////                //return false;
////            }
////            zmsg_destroy(&msg);
////
////            return false;
//        }

        bool RequestPort::Send(capnp::MallocMessageBuilder &message) const {
            if (port_socket_ == nullptr || !is_connected_){
                return false;
            }

            return SenderPort::Send(message);
        }

        RequestPort::~RequestPort() noexcept {

        }
    }
}

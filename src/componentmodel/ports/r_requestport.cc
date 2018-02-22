//
// Created by istvan on 9/30/16.
//

#include <componentmodel/ports/r_requestport.h>


namespace riaps {
    namespace ports {

        RequestPort::RequestPort(const _component_port_req &config, const ComponentBase *parentComponent)
                : PortBase(PortTypes::Request, (component_port_config*)(&config), parentComponent),
                  SenderPort(this),
                  m_capnpReader(capnp::FlatArrayMessageReader(nullptr)) {
            m_port_socket = zsock_new(ZMQ_REQ);

            auto i = zsock_rcvtimeo (m_port_socket);

            int timeout = 500;//msec
            int lingerValue = 0;
            int connectTimeout = 1000; //msec
            zmq_setsockopt(m_port_socket, ZMQ_SNDTIMEO, &timeout , sizeof(int));
            zmq_setsockopt(m_port_socket, ZMQ_LINGER, &lingerValue, sizeof(int));


            m_isConnected = false;
        }



        void RequestPort::Init() {

            const _component_port_req* current_config = GetConfig();

            auto results =
                    subscribeToService(GetParentComponent()->GetActor()->getApplicationName(),
                                         GetParentComponent()->GetConfig().component_name,
                                       GetParentComponent()->GetActor()->getActorName(),
                                         riaps::discovery::Kind::REQ,
                                         (current_config->isLocal?riaps::discovery::Scope::LOCAL:riaps::discovery::Scope::GLOBAL),
                                         current_config->portName, // Subscriber name
                                         current_config->messageType);

            for (auto result : results) {
                std::string endpoint = "tcp://" + result.host_name + ":" + std::to_string(result.port);
                ConnectToResponse(endpoint);
            }
        }

        bool RequestPort::ConnectToResponse(const std::string &rep_endpoint) {
            int rc = zsock_connect(m_port_socket, "%s", rep_endpoint.c_str());

            if (rc != 0) {
                std::cout << "Request '" + GetConfig()->portName + "' couldn't connect to " + rep_endpoint
                          << std::endl;
                return false;
            }

            m_isConnected = true;
            std::cout << "Request port connected to: " << rep_endpoint << std::endl;
            return true;
        }

        const _component_port_req* RequestPort::GetConfig() const{
            return (_component_port_req*)GetPortBaseConfig();
        }

        RequestPort* RequestPort::AsRequestPort() {
            return this;
        }



        bool RequestPort::Recv(capnp::FlatArrayMessageReader** messageReader) {
            zmsg_t* msg = zmsg_recv((void*)GetSocket());

            if (msg){
                //char* msgType = zmsg_popstr(msg);
                //messageType = msgType;
                //if (msgType!=NULL){
                    zframe_t* bodyFrame = zmsg_pop(msg);
                    size_t size = zframe_size(bodyFrame);
                    byte* data = zframe_data(bodyFrame);

                    auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));
                    m_capnpReader = capnp::FlatArrayMessageReader(capnp_data);
                    *messageReader = &m_capnpReader;

                    zframe_destroy(&bodyFrame);
                    return true;
                //}
                //return false;
            }
            zmsg_destroy(&msg);

            return false;
        }

        bool RequestPort::Send(capnp::MallocMessageBuilder &message) const {
            if (m_port_socket == nullptr || !m_isConnected){
                return false;
            }

            zmsg_t* zmsg;
            zmsg << message;

            int rc = zmsg_send(&zmsg, const_cast<zsock_t*>(GetSocket()));
            return rc==0;
        }

        RequestPort::~RequestPort() noexcept {

        }
    }
}

//
// Created by istvan on 9/30/16.
//

#include <componentmodel/ports/r_arequestport.h>



namespace riaps {
    namespace ports {

        AsyncRequestPort::AsyncRequestPort(const _component_port_req &config, const ComponentBase *component)
                : PortBase(PortTypes::Request,
                           (component_port_config*)(&config),
                           component),
                  _capnpReader(capnp::FlatArrayMessageReader(nullptr)) {
            _port_socket = zsock_new(ZMQ_DEALER);
            _socketId = zuuid_new();

            //auto i = zsock_rcvtimeo (_port_socket);

            int timeout = 500;//msec
            int lingerValue = 0;
            int connectTimeout = 1000; //msec
            zmq_setsockopt(_port_socket, ZMQ_SNDTIMEO, &timeout , sizeof(int));
            zmq_setsockopt(_port_socket, ZMQ_LINGER, &lingerValue, sizeof(int));
            zsock_set_identity(_port_socket, zuuid_str(_socketId));

            //i = zsock_rcvtimeo (_port_socket);

            _isConnected = false;
        }



        void AsyncRequestPort::Init() {

            const _component_port_req* current_config = GetConfig();

            auto results =
                    subscribeToService(GetParentComponent()->GetActor()->GetApplicationName(),
                                       GetParentComponent()->GetConfig().component_name,
                                       GetParentComponent()->GetActor()->GetActorName(),
                                       riaps::discovery::Kind::REQ,
                                       (current_config->isLocal?riaps::discovery::Scope::LOCAL:riaps::discovery::Scope::GLOBAL),
                                       current_config->portName, // Subscriber name
                                       current_config->messageType);

            for (auto result : results) {
                std::string endpoint = "tcp://" + result.host_name + ":" + std::to_string(result.port);
                ConnectToResponse(endpoint);
            }
        }

        bool AsyncRequestPort::ConnectToResponse(const std::string &rep_endpoint) {
            int rc = zsock_connect(_port_socket, "%s", rep_endpoint.c_str());

            if (rc != 0) {
                std::cout << "Request '" + GetConfig()->portName + "' couldn't connect to " + rep_endpoint
                          << std::endl;
                return false;
            }

            _isConnected = true;
            std::cout << "Request port connected to: " << rep_endpoint << std::endl;
            return true;
        }

        const _component_port_req* AsyncRequestPort::GetConfig() const{
            return (_component_port_req*)GetPortBaseConfig();
        }

        AsyncRequestPort* AsyncRequestPort::AsAsyncRequestPort() {
            return this;
        }

        bool AsyncRequestPort::Recv(capnp::FlatArrayMessageReader** messageReader) {
            zmsg_t* msg = zmsg_recv((void*)GetSocket());

            if (msg){
                zframe_t* bodyFrame = zmsg_pop(msg);
                size_t size = zframe_size(bodyFrame);
                byte* data = zframe_data(bodyFrame);

                auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));
                _capnpReader = capnp::FlatArrayMessageReader(capnp_data);
                *messageReader = &_capnpReader;

                zframe_destroy(&bodyFrame);
                return true;
            }
            zmsg_destroy(&msg);

            return false;
        }


        const std::string AsyncRequestPort::Send(capnp::MallocMessageBuilder &message, int64_t expiration) const {
            if (_port_socket == nullptr || !_isConnected){
                return "";
            }

            zmsg_t* zmsg;
            zmsg << message;

            // Create the timestamp
            capnp::MallocMessageBuilder tsBuilder;
            auto msgTimestamp = tsBuilder.initRoot<riaps::distrcoord::RiapsTimestamp>();

            // Build the timestamp
            // TODO: ? Another kind of timestamp ? PeterTs ?
            msgTimestamp.setValue(zclock_mono());
            zmsg_t* zmsgTimestamp;
            zmsgTimestamp << tsBuilder;

            // Create expiration time
            capnp::MallocMessageBuilder expBuilder;
            auto msgExpire = expBuilder.initRoot<riaps::distrcoord::RiapsTimestamp>();
            msgExpire.setValue(expiration);
            zmsg_t* zmsgExpiration;
            zmsgExpiration << expBuilder;

            // Generate uniqueId
            std::string msgId;
            {
                auto id = zuuid_new();
                msgId.assign(zuuid_str(id));
                zuuid_destroy(&id);
            }

            int rc = zsock_send(const_cast<zsock_t*>(GetSocket()),
                                "mmsm"        ,
                                zmsgTimestamp ,
                                zmsgExpiration,
                                msgId.c_str() ,
                                zmsg)
            ;

            zmsg_destroy(&zmsgTimestamp);
            zmsg_destroy(&zmsgExpiration);
            zmsg_destroy(&zmsg);
            if (rc == 0) {
                return msgId;
            }
            return "";
        }

        AsyncRequestPort::~AsyncRequestPort() noexcept {
            zuuid_destroy(&_socketId);
        }


    }
}

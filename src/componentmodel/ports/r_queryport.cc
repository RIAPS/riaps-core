//
// Created by istvan on 9/30/16.
//

#include <componentmodel/ports/r_queryport.h>


namespace riaps {
    namespace ports {

        QueryPort::QueryPort(const _component_port_qry &config, const ComponentBase *component)
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



        void QueryPort::Init() {

            const _component_port_qry* current_config = GetConfig();

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

        bool QueryPort::ConnectToResponse(const std::string &ansEndpoint) {
            int rc = zsock_connect(_port_socket, "%s", ansEndpoint.c_str());

            if (rc != 0) {
                std::cout << "Query '" + GetConfig()->portName + "' couldn't connect to " + ansEndpoint
                          << std::endl;
                return false;
            }

            _isConnected = true;
            std::cout << "Query port connected to: " << ansEndpoint << std::endl;
            return true;
        }

        const _component_port_qry* QueryPort::GetConfig() const{
            return (_component_port_qry*)GetPortBaseConfig();
        }

        QueryPort* QueryPort::AsQueryPort() {
            return this;
        }

        bool QueryPort::Recv(capnp::FlatArrayMessageReader** messageReader) {
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


        const std::string QueryPort::Send(capnp::MallocMessageBuilder &message, bool addTimestamp) const {
            if (_port_socket == nullptr || !_isConnected){
                return "";
            }

            zframe_t* userFrame;
            userFrame << message;

            zframe_t* tsFrame = nullptr;
            if (addTimestamp){
                int64_t ztimeStamp = zclock_time();
                tsFrame = zframe_new(&ztimeStamp, sizeof(ztimeStamp));
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
                return msgId;
            }
            return "";
        }

        QueryPort::~QueryPort() noexcept {
            zuuid_destroy(&_socketId);
        }


    }
}

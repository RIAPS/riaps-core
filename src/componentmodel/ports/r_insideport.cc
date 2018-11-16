//
// Created by istvan on 5/17/17.
//

#include <componentmodel/ports/r_insideport.h>

namespace riaps{
    namespace ports{

        InsidePort::InsidePort(const component_port_ins &config, InsidePortMode mode, const ComponentBase *parent_component)
            : PortBase(PortTypes::Inside,
                       (component_port_config*)&config,
                       parent_component),
              SenderPort(this)//,
              //_capnpReader(nullptr)
        {
            _endpoint = "inproc://inside_" + config.port_name;

            if (mode == InsidePortMode::CONNECT){
                port_socket_ = zsock_new_pair(_endpoint.c_str());
            } else {
                port_socket_ = zsock_new(ZMQ_PAIR);
                zsock_bind(port_socket_, "%s", _endpoint.c_str());
            }

            zsock_set_rcvtimeo(port_socket_, 500);
        }

        const component_port_ins* InsidePort::GetConfig() const {
            return (component_port_ins*)GetPortBaseConfig();
        }

        const std::string& InsidePort::GetEndpoint() {
            return _endpoint;
        }


        InsidePort* InsidePort::AsInsidePort() {
            return this;
        }

        bool InsidePort::Recv(zmsg_t** insideMessage) {
            *insideMessage = zmsg_recv((void*)GetSocket());


            return true;
        }

//        bool InsidePort::Recv(riaps::ports::InsideMessage::Reader** insideMessage) {
//            zmsg_t* msg = zmsg_recv((void*)GetSocket());
//
//            if (msg){
//
//                zframe_t* bodyFrame = zmsg_pop(msg);
//                size_t size = zframe_size(bodyFrame);
//                byte* data = zframe_data(bodyFrame);
//
//                auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));
//                _capnpReader = capnp::FlatArrayMessageReader(capnp_data);
//                _insideMessageReader = _capnpReader.getRoot<riaps::ports::InsideMessage>();
//                *insideMessage = &_insideMessageReader;
//
//                zframe_destroy(&bodyFrame);
//                return true;
//
//            }
//            zmsg_destroy(&msg);
//
//            return false;
//        }

        InsidePort::~InsidePort() noexcept {

        }

    }
}
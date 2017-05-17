//
// Created by istvan on 5/17/17.
//

#include <componentmodel/r_insiderport.h>

namespace riaps{
    namespace ports{

        InsidePort::InsidePort(const _component_port_ins_j &config, ComponentBase *parent_component)
            : PortBase(PortTypes::Inside, (component_port_config*)&config) {
            _endpoint = "inproc://inside_" + config.portName;
            _port_socket = zsock_new_pair(_endpoint.c_str());
        }

        const _component_port_ins_j* InsidePort::GetConfig() const {
            return (_component_port_ins_j*)GetPortBaseConfig();
        }

        std::string InsidePort::GetEndpoint() {
            return _endpoint;
        }


        InsidePort* InsidePort::AsInsidePort() {
            return this;
        }

        bool InsidePort::Send(zmsg_t **zmessage) const {
            int rc = zmsg_send(zmessage, _port_socket);
            return rc == 0;
        }

        InsidePort::~InsidePort() {

        }

    }
}
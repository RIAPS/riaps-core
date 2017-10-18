//
// Created by istvan on 10/18/17.
//

#include <componentmodel/r_senderport.h>
#include <componentmodel/r_requestport.h>
#include <utils/r_utils.h>
#include <czmq.h>

namespace riaps{
    namespace ports{


        SenderPort::SenderPort(PortBase* portBase) : _port(portBase) {

        }

        bool SenderPort::Send(capnp::MallocMessageBuilder &message) const {
            zmsg_t* msg = nullptr;
            msg << message;

            int rc = zmsg_send(&msg, const_cast<zsock_t*>(_port->GetSocket()));
            return rc == 0;
        }

        SenderPort::~SenderPort() {

        }
    }
}
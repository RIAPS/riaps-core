//
// Created by istvan on 10/18/17.
//

#include <componentmodel/ports/r_senderport.h>
#include <componentmodel/ports/r_requestport.h>
#include <utils/r_utils.h>
#include <czmq.h>

namespace riaps{
    namespace ports{


        SenderPort::SenderPort(PortBase* portBase) : m_port(portBase) {

        }




        bool SenderPort::Send(capnp::MallocMessageBuilder &message) const {

            zmsg_t* msg = nullptr;
            msg << message;

            return Send(&msg);
        }
        
        bool SenderPort::Send(zmsg_t **message) const {
            if (m_port->GetPortBaseConfig()->isTimed){
                timespec t;
                clock_gettime(CLOCK_REALTIME, &t);
                double tdouble = t.tv_sec + (((float)t.tv_nsec)/BILLION);
                byte* buffer = new byte[sizeof(tdouble)];
                zmsg_addmem(*message, buffer, sizeof(double));
            }
            int rc = zmsg_send(message, const_cast<zsock_t*>(m_port->GetSocket()));
            return rc == 0;
        }

        SenderPort::~SenderPort() {

        }
    }
}
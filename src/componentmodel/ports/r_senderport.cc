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
            if (m_port->config()->is_timed){
                timespec t;
                clock_gettime(CLOCK_REALTIME, &t);
                double tdouble = (float)t.tv_sec + (((float)t.tv_nsec)/1000000000.0);
                byte* buffer = new byte[sizeof(tdouble)];
                memcpy(buffer, &tdouble, sizeof(tdouble));
                zmsg_addmem(*message, buffer, sizeof(double));
            }
            int rc = zmsg_send(message, const_cast<zsock_t*>(m_port->port_socket()));
            return rc == 0;
        }

        SenderPort::~SenderPort() {

        }
    }
}
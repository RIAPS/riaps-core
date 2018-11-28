#include <componentmodel/ports/r_senderport.h>
#include <componentmodel/ports/r_requestport.h>
#include <utils/r_utils.h>
#include <czmq.h>

namespace riaps{
    namespace ports{
        SenderPort::SenderPort(PortBase* port_base) : port_(port_base) {

        }

        PortResult SenderPort::Send(capnp::MallocMessageBuilder &message) const {
            zmsg_t* msg = nullptr;
            msg << message;

            return Send(&msg);
        }

        PortResult SenderPort::Send(zmsg_t **message) const {
            if (port_->config()->is_timed){
                timespec t;
                clock_gettime(CLOCK_REALTIME, &t);
                double tdouble = (float)t.tv_sec + (((float)t.tv_nsec)/1000000000.0);
                byte* buffer = new byte[sizeof(tdouble)];
                memcpy(buffer, &tdouble, sizeof(tdouble));
                zmsg_addmem(*message, buffer, sizeof(double));
            }
            int rc = zmsg_send(message, const_cast<zsock_t*>(port_->port_socket()));
            return PortResult(rc);
        }


        /**
         * Sends the byte array, doesn't destroy the message, it is the caller's responsibility.
         * @param message The byte array to be sent.
         * @param size  Size of the array.
         * @return
         */
        PortResult SenderPort::Send(byte* message, size_t size) const {
            zmsg_t* zmsg = zmsg_new();

            // Copies the byte array
            zmsg_addmem(zmsg, message, size);
            return Send(&zmsg);
        }
    }
}
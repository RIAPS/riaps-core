#include <componentmodel/ports/r_recvport.h>
#include <componentmodel/ports/r_requestport.h>

using namespace std;

namespace riaps{
    namespace ports {
        RecvPort::RecvPort(riaps::ports::PortBase *portBase) :
                port_       (portBase),
                last_frm_   (nullptr) ,
                last_zmsg_  (nullptr) ,
                last_reader_(nullptr) {
        }

        shared_ptr<capnp::FlatArrayMessageReader> RecvPort::Recv() {
            auto socket = port_->port_socket();
            return Recv(const_cast<zsock_t*>(socket));
        }

        shared_ptr<capnp::FlatArrayMessageReader> RecvPort::Recv(zsock_t *socket) {
            zmsg_t* msg = zmsg_recv(socket);
            last_zmsg_ = shared_ptr<zmsg_t>(msg, [](zmsg_t* z){zmsg_destroy(&z);});
            return Recv(msg);
        }

        shared_ptr<capnp::FlatArrayMessageReader> RecvPort::Recv(zmsg_t*msg) {
            auto results = shared_ptr<capnp::FlatArrayMessageReader>(new capnp::FlatArrayMessageReader(nullptr));
            if (msg){
                zframe_t* last_frame = zmsg_pop(msg);

                if (!last_frame) return results;
                last_frm_ = shared_ptr<zframe_t>(last_frame, [](zframe_t* f){zframe_destroy(&f);});
                (*last_frm_) >> results;

                if (port_->config()->is_timed){
                    zframe_t* frm_timestamp = zmsg_pop(msg);
                    if (frm_timestamp!=nullptr) {
                        auto data = zframe_data(frm_timestamp);
                        double send_timestamp;
                        memcpy(&send_timestamp, data, sizeof(double));
                        SetSendTimestamp(send_timestamp);
                        zframe_destroy(&frm_timestamp);
                    }

                    timespec recv_timestamp;;
                    clock_gettime(CLOCK_REALTIME, &recv_timestamp);
                    SetRecvTimestamp(recv_timestamp);
                }
            }
            last_reader_ = results;
            return results;
        }
    }
}
//
// Created by istvan on 3/29/18.
//

#include <componentmodel/ports/r_recvport.h>
#include <componentmodel/ports/r_requestport.h>

namespace riaps{
    namespace ports {
        RecvPort::RecvPort(riaps::ports::PortBase *portBase) :
                m_port(portBase),
                m_lastFrm(nullptr),
                m_lastZmsg(nullptr),
                m_lastCapnpReader(nullptr){

        }

        std::shared_ptr<capnp::FlatArrayMessageReader> RecvPort::Recv() {
            auto socket = m_port->GetSocket();
            return Recv(const_cast<zsock_t*>(socket));
        }

        std::shared_ptr<capnp::FlatArrayMessageReader> RecvPort::Recv(zsock_t *socket) {
            zmsg_t* msg = zmsg_recv(socket);
            m_lastZmsg = std::shared_ptr<zmsg_t>(msg, [](zmsg_t* z){zmsg_destroy(&z);});
            return Recv(msg);
        }

        std::shared_ptr<capnp::FlatArrayMessageReader> RecvPort::Recv(zmsg_t*msg) {
            auto results = std::shared_ptr<capnp::FlatArrayMessageReader>(new capnp::FlatArrayMessageReader(nullptr));
            if (msg){
                zframe_t* lastFrame = zmsg_pop(msg);

                if (!lastFrame) return results;
                m_lastFrm = std::shared_ptr<zframe_t>(lastFrame, [](zframe_t* f){zframe_destroy(&f);});
                (*m_lastFrm) >> results;

                if (m_port->GetPortBaseConfig()->isTimed){
                    zframe_t* frmTimestamp = zmsg_pop(msg);
                    if (frmTimestamp!=nullptr) {
                        auto data = zframe_data(frmTimestamp);
                        double sendTimestamp;
                        memcpy(&sendTimestamp, data, sizeof(double));
                        SetSendTimestamp(sendTimestamp);
                        zframe_destroy(&frmTimestamp);
                    }

                    timespec recvTimestamp;;
                    clock_gettime(CLOCK_REALTIME, &recvTimestamp);
                    SetRecvTimestamp(recvTimestamp);
                }
            }
            m_lastCapnpReader = results;
            return results;
        }
    }
}
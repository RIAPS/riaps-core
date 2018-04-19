//
// Created by istvan on 3/29/18.
//

#ifndef RIAPS_CORE_R_RECVPORT_H
#define RIAPS_CORE_R_RECVPORT_H

#include "r_timedport.h"

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <spdlog/spdlog.h>
#include <czmq.h>
#include <memory>

namespace riaps{
    namespace ports{

        class PortBase;

        class RecvPort : public riaps::ports::TimedPort {
            //static_assert(std::is_base_of<PortBase, T>::value, "T must extend PortBase");
        public:
            RecvPort(PortBase* portBase);

            virtual std::shared_ptr<capnp::FlatArrayMessageReader> Recv();
            virtual std::shared_ptr<capnp::FlatArrayMessageReader> Recv(zsock_t* socket);
            virtual std::shared_ptr<capnp::FlatArrayMessageReader> Recv(zmsg_t* msg);

            virtual ~RecvPort() = default;


        private:
            PortBase*                 m_port;
            std::shared_ptr<zmsg_t>   m_lastZmsg;
            std::shared_ptr<zframe_t> m_lastFrm;
            std::shared_ptr<capnp::FlatArrayMessageReader> m_lastCapnpReader;

        };
    }
}

#endif //RIAPS_CORE_R_RECVPORT_H

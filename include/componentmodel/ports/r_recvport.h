//
// Created by istvan on 3/29/18.
//

#ifndef RIAPS_CORE_R_RECVPORT_H
#define RIAPS_CORE_R_RECVPORT_H

#include "r_timedport.h"
#include <componentmodel/r_messagereader.h>

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <czmq.h>
#include <memory>

namespace riaps{
    namespace ports{

        class PortBase;

        class RecvPort : public riaps::ports::TimedPort {
        public:
            RecvPort(PortBase* portBase);

//            virtual std::shared_ptr<capnp::FlatArrayMessageReader> Recv();
//            virtual std::shared_ptr<capnp::FlatArrayMessageReader> Recv(zsock_t* socket);
//            virtual std::shared_ptr<capnp::FlatArrayMessageReader> Recv(zmsg_t* msg);

            virtual std::tuple<std::unique_ptr<MessageReaderArray>, PortError> Recv();
            virtual std::tuple<std::unique_ptr<MessageReaderArray>, PortError> Recv(zsock_t* socket);
            virtual std::tuple<std::unique_ptr<MessageReaderArray>, PortError> Recv(zmsg_t* msg);

            virtual ~RecvPort() = default;


        private:
            PortBase*                 port_;
            //std::shared_ptr<zmsg_t>   last_zmsg_;
            //std::shared_ptr<zframe_t> last_frm_;
            //std::shared_ptr<capnp::FlatArrayMessageReader> last_reader_;

        };
    }
}

#endif //RIAPS_CORE_R_RECVPORT_H

#ifndef RIAPS_CORE_R_RECVPORT_H
#define RIAPS_CORE_R_RECVPORT_H

#include "r_timedport.h"
#include <componentmodel/r_messagereader.h>

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <czmq.h>
#include <memory>

namespace riaps::ports{
        class PortBase;

        class RecvPort : public riaps::ports::TimedPort {
        public:

            /**
             * @param port_base The port object that is intended to be call the Recv() method.
             */
            explicit RecvPort(PortBase* port_base);

            /**
             * Reads the message from the port_socket_
             * @return A tuple containing the received message as kj::array bytes and an error structure.
             */
            virtual std::tuple<std::unique_ptr<MessageReaderArray>, PortError> Recv();
            ~RecvPort() override = default;

        private:
            PortBase*                 port_;
            std::tuple<std::unique_ptr<MessageReaderArray>, PortError> Recv(zsock_t* socket);
            std::tuple<std::unique_ptr<MessageReaderArray>, PortError> Recv(zmsg_t* msg);
        };
    }

#endif //RIAPS_CORE_R_RECVPORT_H

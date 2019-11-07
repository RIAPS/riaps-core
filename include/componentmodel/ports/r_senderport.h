//
// Created by istvan on 10/18/17.
//

#ifndef RIAPS_CORE_R_SENDERPORT_H
#define RIAPS_CORE_R_SENDERPORT_H

#include "r_porterror.h"

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <czmq.h>

#include <type_traits>
#include <componentmodel/r_messageparams.h>

namespace riaps{
    namespace ports{

        class PortBase;

        /**
         * Base class of all sender type ports: Publisher, Request/Response, Query/Answer
         */
        class SenderPort{
        public:

            /**
             * @param portBase Instance of the sender-type port
             */
            SenderPort(PortBase* portBase);

            /**
             * Sends a capnp message.
             * @param message
             * @return
             */
            virtual PortError Send(capnp::MallocMessageBuilder& message) const;

            /**
             * Sends a ZMQ message on the port. Ownership is taken, the message is deleted by the Send()
             * @param message ZMQ message to be sent.
             * @return PortError
             */
            virtual PortError Send(zmsg_t** message) const;

            /**
             * Sends a byte array. Ownership is taken, the buffer is deleted by the Send()
             * @param message The message represented in byte array.
             * @param size Size if the buffer.
             * @return PortError
             */
            virtual PortError Send(byte* message, size_t size) const;


            virtual ~SenderPort() = default;

        private:
            PortBase* port_;
        };
    }
}

#endif //RIAPS_CORE_R_SENDERPORT_H

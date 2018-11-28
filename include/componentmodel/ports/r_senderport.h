//
// Created by istvan on 10/18/17.
//

#ifndef RIAPS_CORE_R_SENDERPORT_H
#define RIAPS_CORE_R_SENDERPORT_H

#include "r_portresult.h"

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <czmq.h>

#include <type_traits>
#include <componentmodel/r_payload.h>

namespace riaps{
    namespace ports{

        class PortBase;

        class SenderPort{
            //static_assert(std::is_base_of<PortBase, T>::value, "T must extend PortBase");
        public:
            SenderPort(PortBase* portBase);

            virtual PortResult Send(capnp::MallocMessageBuilder& message) const;

            // TODO: Redesign. We don't want to expose this to the developers.
            virtual PortResult Send(zmsg_t** message) const;
            virtual PortResult Send(byte* message, size_t size) const;


            virtual ~SenderPort() = default;

        private:
            PortBase* port_;
        };
    }
}

#endif //RIAPS_CORE_R_SENDERPORT_H

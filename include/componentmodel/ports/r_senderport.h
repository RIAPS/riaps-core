//
// Created by istvan on 10/18/17.
//

#ifndef RIAPS_CORE_R_SENDERPORT_H
#define RIAPS_CORE_R_SENDERPORT_H

//#include <componentmodel/ports/r_portbase.h>

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

            virtual bool Send(capnp::MallocMessageBuilder& message) const;
            virtual bool Send(zmsg_t** message) const;
            
            virtual ~SenderPort();


        private:
            PortBase* m_port;
        };
    }
}

#endif //RIAPS_CORE_R_SENDERPORT_H

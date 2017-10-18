//
// Created by istvan on 10/18/17.
//

#ifndef RIAPS_CORE_R_SENDERPORT_H
#define RIAPS_CORE_R_SENDERPORT_H

#include <componentmodel/r_portbase.h>

#include <capnp/message.h>
#include <capnp/serialize.h>

#include <type_traits>

namespace riaps{
    namespace ports{

        //class PortBase;

        class SenderPort{
            //static_assert(std::is_base_of<PortBase, T>::value, "T must extend PortBase");
        public:
            SenderPort(PortBase* portBase);

            virtual bool Send(capnp::MallocMessageBuilder& message) const;
            virtual ~SenderPort();


        private:
            PortBase* _port;
        };
    }
}

#endif //RIAPS_CORE_R_SENDERPORT_H

//
// Created by istvan on 5/17/17.
//

#ifndef RIAPS_CORE_R_INSIDER_H
#define RIAPS_CORE_R_INSIDER_H

#include "r_portbase.h"
#include <messaging/insideport.capnp.h>
#include <capnp/serialize.h>
#include <capnp/message.h>

namespace riaps {

    class ComponentBase;


    namespace ports {

        enum InsidePortMode {CONNECT, BIND};

        class InsidePort : public PortBase {
        public:
            using PortBase::Send;

            InsidePort(const _component_port_ins_j &config, InsidePortMode mode, ComponentBase *parent_component);

            std::string GetEndpoint();

            virtual const _component_port_ins_j* GetConfig() const;

            virtual InsidePort*  AsInsidePort() ;

            virtual bool Send(zmsg_t** zmessage) const;
            virtual bool Recv(riaps::ports::InsideMessage::Reader** insideMessage);

            ~InsidePort() noexcept ;

        protected:
            std::string _endpoint;
            capnp::FlatArrayMessageReader       _capnpReader;
            riaps::ports::InsideMessage::Reader _insideMessageReader;

        };
    }
}

#endif //RIAPS_CORE_R_INSIDER_H

//
// Created by istvan on 5/17/17.
//

#ifndef RIAPS_CORE_R_INSIDE_H
#define RIAPS_CORE_R_INSIDE_H

#include <componentmodel/ports/r_portbase.h>
#include <componentmodel/ports/r_senderport.h>
//#include <messaging/insideport.capnp.h>
#include <capnp/serialize.h>
#include <capnp/message.h>

namespace riaps {

    class ComponentBase;


    namespace ports {

        enum InsidePortMode {CONNECT, BIND};

        class InsidePort : public PortBase, public SenderPort {
        public:
            InsidePort(const ComponentPortIns &config, InsidePortMode mode, const ComponentBase *parent_component);
            const std::string& GetEndpoint();
            virtual const ComponentPortIns* GetConfig() const;
            virtual bool Recv(zmsg_t** insideMessage);

            ~InsidePort() noexcept ;

        protected:
            std::string endpoint_;
        };
    }
}

#endif //RIAPS_CORE_R_INSIDE_H

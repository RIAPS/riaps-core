#ifndef RIAPS_FW_R_RESPONSEPORT_H
#define RIAPS_FW_R_RESPONSEPORT_H

#include <componentmodel/ports/r_portbase.h>
#include "r_recvport.h"
#include <componentmodel/r_componentbase.h>
#include <componentmodel/r_configuration.h>

#include "czmq.h"

#include <string>

namespace riaps {

    class ComponentBase;

    namespace ports {

        class ResponsePort : public PortBase, public SenderPort, public RecvPort {
        public:

            ResponsePort(const component_port_rep &config, const ComponentBase *parent);
            virtual const component_port_rep* GetConfig() const;

            virtual ResponsePort* AsResponsePort() ;
            virtual RecvPort*     AsRecvPort()     ;

            ~ResponsePort() noexcept ;


        protected:
            int port_;
            std::string host_;
            std::string endpoint_;
        };
    }
}

#endif //RIAPS_FW_R_RESPONSEPORT_H

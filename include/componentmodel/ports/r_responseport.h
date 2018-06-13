//
// Created by parallels on 9/29/16.
//

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
    //class PortBase;

    namespace ports {

        //class PortBase;

        class ResponsePort : public PortBase, public SenderPort, public RecvPort {
        public:

            ResponsePort(const component_port_rep &config, const ComponentBase *parent_component);
            virtual const component_port_rep* GetConfig() const;

            virtual ResponsePort* AsResponsePort() ;
            virtual RecvPort*     AsRecvPort()     ;

            ~ResponsePort() noexcept ;


        protected:
            int m_port;
            std::string m_host;
            std::string m_endpoint;
        };
    }
}

#endif //RIAPS_FW_R_RESPONSEPORT_H

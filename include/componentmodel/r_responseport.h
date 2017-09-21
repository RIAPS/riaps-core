//
// Created by parallels on 9/29/16.
//

#ifndef RIAPS_FW_R_RESPONSEPORT_H
#define RIAPS_FW_R_RESPONSEPORT_H

#include "r_portbase.h"
#include "r_componentbase.h"
#include "r_configuration.h"

#include "czmq.h"

#include <string>

namespace riaps {

    class ComponentBase;
    //class PortBase;

    namespace ports {

        //class PortBase;

        class ResponsePort : public PortBase {
        public:
            using PortBase::Send;

            ResponsePort(const _component_port_rep &config, ComponentBase *parent_component);
            virtual const _component_port_rep* GetConfig() const;


            //virtual bool Send(std::string& message) const;
            //virtual bool Send(std::vector<std::string>& message) const;

            virtual ResponsePort* AsResponsePort() ;

            ~ResponsePort() noexcept ;


        protected:
            int _port;
            std::string _host;
            std::string _endpoint;

            virtual bool Send(zmsg_t** zmessage) const;
        };
    }
}

#endif //RIAPS_FW_R_RESPONSEPORT_H

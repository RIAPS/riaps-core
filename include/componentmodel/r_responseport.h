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
            ResponsePort(const _component_port_rep_j &config, ComponentBase *parent_component);
            virtual const _component_port_rep_j* GetConfig() const;

            virtual bool Send(zmsg_t** msg) const;
            virtual bool Send(std::string msg) const;

            virtual ResponsePort* AsResponsePort();

            ~ResponsePort();


        protected:
            int _port;
            std::string _host;
            std::string _endpoint;
        };
    }
}

#endif //RIAPS_FW_R_RESPONSEPORT_H

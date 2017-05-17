//
// Created by istvan on 5/17/17.
//

#ifndef RIAPS_CORE_R_INSIDER_H
#define RIAPS_CORE_R_INSIDER_H

#include "r_portbase.h"

namespace riaps {

    class ComponentBase;


    namespace ports {

        class InsidePort : public PortBase {
        public:
            using PortBase::Send;

            InsidePort(const _component_port_ins_j &config, ComponentBase *parent_component);

            std::string GetEndpoint();

            virtual const _component_port_ins_j* GetConfig() const;

            virtual InsidePort*  AsInsidePort() ;

            ~InsidePort();

        protected:
            std::string _endpoint;

            virtual bool Send(zmsg_t** zmessage) const;
        };
    }
}

#endif //RIAPS_CORE_R_INSIDER_H

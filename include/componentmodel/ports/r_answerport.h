//
// Created by parallels on 9/29/16.
//

#ifndef RIAPS_FW_R_ANSWERPORT_H
#define RIAPS_FW_R_ANSWERPORT_H

#include <componentmodel/ports/r_portbase.h>
#include <componentmodel/r_componentbase.h>
#include <componentmodel/r_configuration.h>

#include "czmq.h"

#include <string>

namespace riaps {

    class ComponentBase;
    //class PortBase;

    namespace ports {

        //class PortBase;

        class AnswerPort : public PortBase, public SenderPort {
        public:
            //using PortBase::Send;

            AnswerPort(const component_port_ans &config, const ComponentBase *parent_component);
            virtual const component_port_ans* GetConfig() const;

            bool SendAnswer(capnp::MallocMessageBuilder& builder, std::shared_ptr<MessageParams> params);

            virtual AnswerPort* AsAnswerPort() ;

            ~AnswerPort() noexcept ;


        protected:
            int m_port;
            std::string m_host;
            std::string m_endpoint;

            //virtual bool Send(zmsg_t** zmessage) const;
        };
    }
}

#endif //RIAPS_FW_R_ANSWERPORT_H

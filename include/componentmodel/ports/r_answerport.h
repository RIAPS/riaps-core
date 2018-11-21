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

    namespace ports {

        class AnswerPort : public PortBase, public SenderPort {
        public:
            AnswerPort(const ComponentPortAns &config, const ComponentBase* parent);
            virtual const ComponentPortAns* GetConfig() const;

            bool SendAnswer(capnp::MallocMessageBuilder& builder, std::shared_ptr<MessageParams> params);

            //virtual AnswerPort* AsAnswerPort() ;

            ~AnswerPort() noexcept ;
        protected:
            int         port_;
            std::string host_;
            std::string endpoint_;
        };
    }
}

#endif //RIAPS_FW_R_ANSWERPORT_H

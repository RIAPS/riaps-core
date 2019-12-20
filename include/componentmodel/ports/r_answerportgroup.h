#ifndef RIAPS_FW_R_ANSWERPORTGROUP_H
#define RIAPS_FW_R_ANSWERPORTGROUP_H

#include <componentmodel/ports/r_portbase.h>
#include <componentmodel/r_componentbase.h>
#include <componentmodel/r_configuration.h>

#include "czmq.h"

#include <string>
#include <memory>

namespace riaps {

    class ComponentBase;

    /*
     * RIAPS Ports.
     */
    namespace ports {

        class AnswerPortGroup : public AnswerPort {
        public:
            /**
             * @param config Config parameters.
             * @param parent Component instance which owns the port.
             */
            AnswerPortGroup(const ComponentPortAns &config, const ComponentBase* parent);

            void Init() override;
        };
    }
}

#endif //RIAPS_FW_R_ANSWERPORTGROUP_H

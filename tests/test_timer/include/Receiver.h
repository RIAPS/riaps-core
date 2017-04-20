#ifndef RIAPS_FW_GENERATOR_H
#define RIAPS_FW_GENERATOR_H

#include <libsoc_pwm.h>
#include <libsoc_debug.h>

#include "base/ReceiverBase.h"

namespace timertest {
    namespace components {

        class Receiver : public ReceiverBase {

        public:

            Receiver(_component_conf_j &config, riaps::Actor &actor);


            void OnSignalValue(const messages::SignalValue::Reader &message,
                               riaps::ports::PortBase *port);


            virtual ~Receiver();

        private:
            double _phase = 0.0;
            pwm*   _pwm_output;
        };
    }
}

extern "C" riaps::ComponentBase* create_component(_component_conf_j&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);


#endif

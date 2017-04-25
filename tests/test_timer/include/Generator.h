#ifndef RIAPS_FW_GENERATOR_H
#define RIAPS_FW_GENERATOR_H

#include <libsoc_pwm.h>
#include <libsoc_debug.h>

#include "base/GeneratorBase.h"

namespace timertest {
    namespace components {

        class Generator : public GeneratorBase {

        public:

            Generator(_component_conf_j &config, riaps::Actor &actor);


            virtual void OnClock(riaps::ports::PortBase *port);

            void OnOneShotTimer(const std::string& timerid);

            virtual ~Generator();

        private:
            double    _phase = 0.0;
            pwm*      _pwm_output;
            uint64_t  _cycle = 0;
        };
    }
}

extern "C" riaps::ComponentBase* create_component(_component_conf_j&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);


#endif

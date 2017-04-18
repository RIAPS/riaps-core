#ifndef RIAPS_FW_GENERATOR_H
#define RIAPS_FW_GENERATOR_H

#include "base/GeneratorBase.h"

namespace timertest {
    namespace components {

        class Generator : public GeneratorBase {

        public:

            Generator(_component_conf_j &config, riaps::Actor &actor);


            virtual void OnClock(riaps::ports::PortBase *port);


            virtual ~Generator();

        private:
            std::array<std::chrono::time_point<std::chrono::high_resolution_clock>, 500> _results;
            int _counter=0;
        };
    }
}

extern "C" riaps::ComponentBase* create_component(_component_conf_j&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);


#endif

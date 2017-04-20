#ifndef RIAPS_FW_GENERATOR_H
#define RIAPS_FW_GENERATOR_H


#include <libsoc_debug.h>
#include <libsoc_gpio.h>
#define PPS_OUTPUT  22

#include "base/ReceiverBase.h"

namespace timertest {
    namespace components {

        class Receiver : public ReceiverBase {

        public:

            Receiver(_component_conf_j &config, riaps::Actor &actor);


            void OnSignalValue(const messages::SignalValue::Reader &message,
                               riaps::ports::PortBase *port);

            void OnOneShotTimer(const std::string &timerid);

            virtual ~Receiver();

        private:
            double _lastValue = 0.0;
            //pwm*   _pwm_output;
            gpio*  _pps_output;


//            std::array<timespec, 500> predicted;
//            std::array<timespec, 500> current;
//            std::array<timespec, 500> called;
//            int idx = 0;


        };
    }
}

extern "C" riaps::ComponentBase* create_component(_component_conf_j&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);


#endif

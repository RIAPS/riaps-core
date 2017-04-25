#ifndef RIAPS_FW_GENERATOR_H
#define RIAPS_FW_GENERATOR_H


#include <libsoc_debug.h>
#include <libsoc_gpio.h>
#define PPS_OUTPUT  22

#include "base/ReceiverBase.h"

#define DEBUG_SAMPLES 200

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
            double   _lastValue = 0.0;
            timespec _lastTimestamp;
            gpio*    _pps_output;
            bool     _isHigh = false;


            //std::array<timespec, DEBUG_SAMPLES> _timestamp;
            //std::array<timespec, DEBUG_SAMPLES> _predicted;
            //std::array<timespec, DEBUG_SAMPLES> _triggered;
            //std::array<double, DEBUG_SAMPLES> _lastVal;
            //std::array<double, DEBUG_SAMPLES> _currVal;
            //int idx = 0;


        };
    }
}

extern "C" riaps::ComponentBase* create_component(_component_conf_j&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);


#endif

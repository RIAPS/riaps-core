//
// Created by istvan on 11/11/16.
//

#include "Receiver.h"
#include <capnp/serialize.h>
#include <capnp/message.h>

#define SAMPLING_RATE 1000 // Hz
#define SIGNAL_FREQ 60 // Hz
#define PWM_PERIOD 4000 // nanoseconds


#define SAMPLING_INTERVAL (1000000000L / SAMPLING_RATE) // nanoseconds
#define DPHASE (2 * M_PI * SIGNAL_FREQ / SAMPLING_RATE)

#define PWM_OUTPUT_CHIP 0
#define PWM_CHIP_OUTPUT 0


namespace timertest {
    namespace components {

        Receiver::Receiver(_component_conf_j &config, riaps::Actor &actor) : ReceiverBase(config, actor) {


            _pps_output = libsoc_gpio_request(PPS_OUTPUT, LS_SHARED);
            if (!_pps_output) {
                perror("unable to request gpio pin:");
                exit(-1);
            }
            libsoc_gpio_set_direction(_pps_output, OUTPUT);

            if (libsoc_gpio_get_direction(_pps_output) != OUTPUT)
            {
                perror("unable to set output direction:");
                exit(-1);
            }

        }

        void Receiver::OnSignalValue(const messages::SignalValue::Reader &message, riaps::ports::PortBase *port) {
            auto currentValue     = message.getVal();



            if (_lastValue<0 && currentValue>0){
                auto currentTimestamp = message.getTimestamp();

                int64_t predictNsec = currentTimestamp.getNsec() + 20000000l;
                int64_t predictSec  = currentTimestamp.getSec();

                if (predictNsec>1000000000l){
                    predictSec++;
                    predictNsec-=1000000000l;
                }

                timespec predSpec={predictSec,predictNsec};

                if (CreateOneShotTimer("predTimer", predSpec)){

                }


            }
            _lastValue = currentValue;


        }


        void Receiver::OnOneShotTimer(const std::string &timerid) {

            libsoc_gpio_set_level(_pps_output, HIGH);
            libsoc_gpio_set_level(_pps_output, LOW);

        }



        Receiver::~Receiver() {
            libsoc_gpio_free(_pps_output);
        }
    }
}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    auto result = new timertest::components::Receiver(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
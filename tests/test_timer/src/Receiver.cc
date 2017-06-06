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

#define BILLION 1000000000l


namespace timertest {
    namespace components {

        Receiver::Receiver(_component_conf_j &config, riaps::Actor &actor) : ReceiverBase(config, actor) {
            _pps_output = libsoc_gpio_request(PPS_OUTPUT, LS_GPIO_SHARED);
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
            auto capnpTimestamp = message.getTimestamp();
            auto tsCurrentTimestamp = timespec{capnpTimestamp.getSec(), capnpTimestamp.getNsec()};


            if (_lastValue<0 && currentValue>0){
                double alastval = fabs(asin(_lastValue));
                double acurrval = fabs(asin(currentValue));


                double m = alastval/(acurrval+alastval);

                timespec tsDiff;
                tsDiff.tv_sec = tsCurrentTimestamp.tv_sec - _lastTimestamp.tv_sec;
                tsDiff.tv_nsec = tsCurrentTimestamp.tv_nsec - _lastTimestamp.tv_nsec;

                if (tsDiff.tv_nsec<0){
                    tsDiff.tv_sec--;
                    tsDiff.tv_nsec+=BILLION;
                }
                int nsDiff = tsDiff.tv_sec*BILLION + tsDiff.tv_nsec;
                int tsOffset = m*nsDiff;
                timespec predTs;
                predTs.tv_nsec = _lastTimestamp.tv_nsec + tsOffset + 16666666+1836650; // RC filter phase shift at 60Hz
                predTs.tv_sec = _lastTimestamp.tv_sec;

                while (predTs.tv_nsec>BILLION){
                    predTs.tv_sec++;
                    predTs.tv_nsec-=BILLION;
                }

                clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &predTs, NULL);
                if (_isHigh){
                    _isHigh = false;
                    libsoc_gpio_set_level(_pps_output, LOW);
                } else {
                    _isHigh = true;
                    libsoc_gpio_set_level(_pps_output, HIGH);
                }


            }
            _lastValue = currentValue;
            _lastTimestamp = tsCurrentTimestamp;
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

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
            _pwm_output = libsoc_pwm_request(PWM_OUTPUT_CHIP, PWM_CHIP_OUTPUT, LS_SHARED);
            _pwm_output = libsoc_pwm_request(PWM_OUTPUT_CHIP, PWM_CHIP_OUTPUT, LS_SHARED);
            if (!_pwm_output) {
                perror("unable to request PWM pin:");
                fprintf(stderr, "make sure, you enabled the PWM overlay:\n\techo BB-PWM1 >  /sys/devices/platform/bone_capemgr/slots\n");
                exit(-1);
            }

            libsoc_pwm_set_enabled(_pwm_output, ENABLED);
            if (!libsoc_pwm_get_enabled(_pwm_output))
            {
                perror("unable to enable PWM pin:");
                exit(-1);
            }


            libsoc_pwm_set_polarity(_pwm_output, NORMAL);
            if (libsoc_pwm_get_polarity(_pwm_output) != NORMAL)
            {
                perror("unable to set PWM polarity:");
                exit(-1);
            }

            libsoc_pwm_set_period(_pwm_output, PWM_PERIOD);
            if (libsoc_pwm_get_period(_pwm_output) != PWM_PERIOD)
            {
                perror("unable to set PWM period:");
                exit(-1);
            }
        }

        void Receiver::OnSignalValue(const messages::SignalValue::Reader &message, riaps::ports::PortBase *port) {
            //std::cout << "Value: " << message.getVal() << std::endl;
            libsoc_pwm_set_duty_cycle(_pwm_output, message.getVal() );
        }



        Receiver::~Receiver() {
            libsoc_pwm_free(_pwm_output);
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
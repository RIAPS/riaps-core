//
// Created by istvan on 11/11/16.
//

#include "Generator.h"
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

        Generator::Generator(_component_conf_j &config, riaps::Actor &actor) : GeneratorBase(config, actor) {
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

        void Generator::OnClock(riaps::ports::PortBase *port) {

//            if (_counter < 500)
//                _results[_counter++] = std::chrono::high_resolution_clock::now();
//            else {
//                std::fstream _logStream;
//                _logStream.open("./results.log", std::fstream::out);
//                for (int i =1; i<500; i++){
//                    auto d = std::chrono::duration_cast<std::chrono::nanoseconds>(_results[i]-_results[i-1]).count();
//                    _logStream << d << std::endl;
//                }
//                _logStream.close();
//                kill(getpid(), SIGKILL);
//            }
//            capnp::MallocMessageBuilder messageBuilder;
//            auto msgSensorReady = messageBuilder.initRoot<messages::SensorReady>();
//            msgSensorReady.setMsg("data_ready");
//
//            SendReady(messageBuilder, msgSensorReady);
        }



        Generator::~Generator() {

        }
    }
}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    auto result = new timertest::components::Generator(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
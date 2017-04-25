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
            auto capnpTimestamp = message.getTimestamp();
            auto tsCurrentTimestamp = timespec{capnpTimestamp.getSec(), capnpTimestamp.getNsec()};


            if (_lastValue<0 && currentValue>0){
                //timespec now;
                //clock_gettime(CLOCK_REALTIME, &now);

                //_current[idx] = now;

                //_lastVal[idx] = _lastValue;
                //_currVal[idx] = currentValue;

                double alastval = fabs(asin(_lastValue));
                double acurrval = fabs(asin(currentValue));


                //_timestamp[idx] = tsCurrentTimestamp;

                double m = alastval/(acurrval+alastval);
//                std::cout << m <<std::endl;
//                double tsLastD = _lastTimestamp.tv_sec + ((double)_lastTimestamp.tv_nsec)/1000000000.0;
//                double tsCurrD = tsCurrentTimestamp.tv_sec + ((double)tsCurrentTimestamp.tv_nsec)/1000000000.0;
//
//                double tsDiffD = tsLastD - tsCurrD;
//                double tsRatio = m*tsDiffD;
//
//                timespec predSpec={tsCurrentTimestamp.tv_sec + (time_t)tsRatio, tsCurrentTimestamp.tv_nsec};



                //uint64_t lastTsInNs = _lastTimestamp.tv_sec*1000000000 + _lastTimestamp.tv_nsec;
                //uint64_t currTsInNs = tsCurrentTimestamp.tv_sec*1000000000 + tsCurrentTimestamp.tv_nsec;

                //uint64_t tsDiff  = currTsInNs - lastTsInNs;

                timespec tsDiff;
                tsDiff.tv_sec = tsCurrentTimestamp.tv_sec - _lastTimestamp.tv_sec;
                tsDiff.tv_nsec = tsCurrentTimestamp.tv_nsec - _lastTimestamp.tv_nsec;

                if (tsDiff.tv_nsec<0){
                    tsDiff.tv_sec--;
                    tsDiff.tv_nsec+=BILLION;
                }
//
                int nsDiff = tsDiff.tv_sec*BILLION + tsDiff.tv_nsec;
//
                int tsOffset = m*nsDiff;
//
                timespec predTs;
                //predTs.tv_sec = _lastTimestamp + tsOffset
                predTs.tv_nsec = _lastTimestamp.tv_nsec + tsOffset + 16666666+1836650; // RC filter phase shift at 60Hz
                predTs.tv_sec = _lastTimestamp.tv_sec;

                while (predTs.tv_nsec>BILLION){
                    predTs.tv_sec++;
                    predTs.tv_nsec-=BILLION;
                }


//

//
//                int64_t predictNsec = _lastTimestamp.tv_nsec + predTs.tv_nsec + 16666666;
//                int64_t predictSec  = _lastTimestamp.tv_sec + predTs.tv_sec;
//
                //_lastTimestamp = tsCurrentTimestamp;
//                if (predictNsec>1000000000l){
//                    predictSec++;
//                    predictNsec-=1000000000l;
//                }
//
               // timespec predSpec={predictSec,predictNsec};

                //_predicted[idx] = predTs;
                clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &predTs, NULL);
                //clock_gettime(CLOCK_REALTIME, &now);
                //_triggered[idx] = now;
                libsoc_gpio_set_level(_pps_output, HIGH);
                libsoc_gpio_set_level(_pps_output, LOW);
                //clock_gettime(CLOCK_REALTIME, &now);

//                if (++idx == DEBUG_SAMPLES){
//                    std::fstream _log;
//                    _log.open("thetimes.log", std::fstream::out);
//                    for (int i=0; i< DEBUG_SAMPLES; i++){
//                        _log << "current:\t" << _current[i].tv_sec << "\t" << _current[i].tv_nsec << std::endl;
//                        _log << "timstmp:\t" << _timestamp[i].tv_sec << "\t" << _timestamp[i].tv_nsec << std::endl;
//                        _log << "predict:\t" << _predicted[i].tv_sec << "\t" << _predicted[i].tv_nsec << std::endl;
//                        _log << "triggrd:\t" << _triggered[i].tv_sec << "\t" << _triggered[i].tv_nsec << std::endl;
//                        _log << "lastval:\t" << _lastVal[i] << std::endl;
//                        _log << "currval:\t" << _currVal[i] << std::endl;
//                        _log << std::endl;
//                    }
//                    _log.close();
//                    exit(-1);
//                }
                //libsoc_gpio_set_level(_pps_output, HIGH);
                //libsoc_gpio_set_level(_pps_output, LOW);

                //if (CreateOneShotTimer("predTimer", predSpec)){
                    //std::cout << "Nowwwww: " << now.tv_sec << " " << now.tv_nsec << std::endl;
                    //std::cout << "Timstmp: " << currentTimestamp.getSec() << " " << currentTimestamp.getNsec() << std::endl;
                    //std::cout << "Created: " << predSpec.tv_sec << " " << predSpec.tv_nsec << std::endl;
                //} else{
                    //std::cout << "NoTimer: " << now.tv_sec << " " << now.tv_nsec << std::endl;
                //}



            }
            _lastValue = currentValue;
            _lastTimestamp = tsCurrentTimestamp;


        }


        void Receiver::OnOneShotTimer(const std::string &timerid) {
            //timespec now;
            //clock_gettime(CLOCK_REALTIME, &now);

            //std::cout << "Triggrd: " << now.tv_sec << " " << now.tv_nsec << std::endl;

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
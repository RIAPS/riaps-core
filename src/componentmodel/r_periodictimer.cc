//
// Created by istvan on 5/2/17.
//

#include <componentmodel/r_periodictimer.h>

#include <chrono>
#include <thread>
#include <functional>
#include <atomic>

namespace riaps {

    namespace ports {

        void ptimeractor(zsock_t* pipe, void* args){
            PeriodicTimer* periodicTimer = (PeriodicTimer*)args;

            zpoller_t* poller = zpoller_new(pipe, NULL);
            assert(poller);

            zpoller_set_nonstop(poller, true);
            //zpoller_ignore_interrupts (poller);

            zsock_signal (pipe, 0);

            bool terminated = false;
            bool started    = false;

            zsock_t* _zsock_timer = zsock_new_push(periodicTimer->GetTimerResponseChannel().c_str());
            int lingerValue = 0;
            int sendtimeout = 0; // 0 - returns immediately with EAGAIN if the message cannot be sent
            zmq_setsockopt(_zsock_timer, ZMQ_LINGER,   &lingerValue, sizeof(int));
            zmq_setsockopt(_zsock_timer, ZMQ_SNDTIMEO, &sendtimeout, sizeof(int));

            auto now = std::chrono::high_resolution_clock::now();

            while (!terminated) {

                if (started) {
                    // Send FIRE message
                    zmsg_t *msg = zmsg_new();
                    auto portName = periodicTimer->GetPortName();
                    zmsg_addstr(msg, portName.c_str());

                    auto diff = std::chrono::milliseconds(periodicTimer->GetInterval());
                    now += diff;
                    std::this_thread::sleep_until(now);
                    zmsg_send(&msg, _zsock_timer);
                } else{
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }

                void *which = zpoller_wait(poller, 0);
                if (which == pipe) {
                    zmsg_t *msg = zmsg_recv(which);
                    if (!msg) {
                        std::cout << "No msg => interrupted" << std::endl;
                        break;
                    }

                    char *command = zmsg_popstr(msg);

                    if (streq(command, "$TERM")) {
                        std::cout << "$TERM arrived in periodic timer" << std::endl;
                        terminated = true;
                    } else if (streq(command, "$START")){
                        started = true;
                        now = std::chrono::high_resolution_clock::now();
                    }
                    zstr_free(&command);
                    zmsg_destroy(&msg);
                }
            }
            zsock_destroy(&_zsock_timer);
            zpoller_destroy(&poller);
        }


        PeriodicTimer::PeriodicTimer(std::string &timerresponsechannel, const _component_port_tim &config)
                : PortBase(PortTypes::Timer,
                  (component_port_config * ) & config),
                  _timerresponsechannel(timerresponsechannel) {
            _interval = config.period;
            _periodicTimerActor = zactor_new(ptimeractor, this);
        }

        std::string& PeriodicTimer::GetTimerResponseChannel() {
            return _timerresponsechannel;
        }

        int PeriodicTimer::GetInterval() {
            return _interval;
        }

        void PeriodicTimer::start() {
            zmsg_t* msg = zmsg_new();

            zmsg_addstr(msg,"$START");
            zactor_send(_periodicTimerActor, &msg);

        }

        void PeriodicTimer::stop() {
            zactor_destroy(&_periodicTimerActor);
        }

        const zsock_t* PeriodicTimer::GetSocket() const {
            return NULL;
        }

        PeriodicTimer* PeriodicTimer::AsTimerPort() {
            return this;
        }

        PeriodicTimer::~PeriodicTimer(){

        }
    }
}
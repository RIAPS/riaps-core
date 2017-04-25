//
// Created by parallels on 9/19/16.
//

#include "componentmodel/r_timer.h"

namespace riaps {

    namespace ports {


        CallBackTimer::CallBackTimer(std::string &timerresponsechannel, const _component_port_tim_j& config)
                : PortBase(PortTypes::Timer, (component_port_config*)&config),_execute(false) {
            _zsock_timer = zsock_new_push(timerresponsechannel.c_str());
        }

        CallBackTimer::~CallBackTimer() {
            if (_execute.load(std::memory_order_acquire)) {
                stop();
            };
            zsock_destroy(&_zsock_timer);
        }

        CallBackTimer* CallBackTimer::AsTimerPort() {
            return this;
        }

        const zsock_t *CallBackTimer::GetSocket() const {
            return _zsock_timer;
        }

        void CallBackTimer::stop() {
            _execute.store(false, std::memory_order_release);
            if (_thd.joinable())
                _thd.join();
        }

        void CallBackTimer::start(int interval) {
            if (_execute.load(std::memory_order_acquire)) {
                stop();
            };
            _execute.store(true, std::memory_order_release);
            _thd = std::thread([this, interval]() {
                //timespec now;
                //clock_gettime(CLOCK_REALTIME, &now);

                auto now = std::chrono::high_resolution_clock::now();

                while (_execute.load(std::memory_order_acquire)) {



                    // Send FIRE message
                    zmsg_t *msg = zmsg_new();
                    auto portName = GetPortName();
                    zmsg_addstr(msg, portName.c_str());

                    auto diff = std::chrono::milliseconds(interval);
                    now+=diff;


//                    timespec wakeUp;
//                    wakeUp.tv_nsec = now.tv_nsec + interval*1000000;
//                    wakeUp.tv_sec  = now.tv_sec;
//
//                    if ( wakeUp.tv_nsec>1000000000l){
//                        wakeUp.tv_sec++;
//                        wakeUp.tv_nsec-=1000000000l;
//                    }

                    //std::cout << "AtS: " << now.tv_sec << " " << now.tv_nsec << std::endl;

                    //clock_gettime(CLOCK_REALTIME, &now);
                   // std::cout << "Now: " << now.tv_sec << " " << now.tv_nsec << std::endl;
                    //std::cout << "Wke: " << wakeUp.tv_sec << " " << wakeUp.tv_nsec << std::endl;

                    //std::this_thread::sleep_until()

                    //exit(-1);

                    std::this_thread::sleep_until(now);

//                    int rc = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &wakeUp, NULL);
//                    if (rc!=0) {
//                        perror("rc is not 0 but " + rc);
//                        exit(-1);
//                    }
                    zmsg_send(&msg, this->_zsock_timer);


                    //std::this_thread::sleep_for(
                    //        std::chrono::milliseconds(interval));
                }
            });
        }

        bool CallBackTimer::is_running() const noexcept {
            return (_execute.load(std::memory_order_acquire) &&
                    _thd.joinable());
        }
    }

}
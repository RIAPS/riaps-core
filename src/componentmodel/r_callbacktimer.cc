//
// Created by parallels on 9/19/16.
//

#include "componentmodel/r_callback.h"

namespace riaps {

    namespace ports {


        CallBackTimer::CallBackTimer(std::string &timerresponsechannel, const _component_port_tim_j& config)
                : PortBase(PortTypes::Timer, (component_port_config*)&config),_execute(false),_timerresponsechannel(timerresponsechannel) {

        }

        CallBackTimer::~CallBackTimer() {
            //if (_execute.load(std::memory_order_acquire)) {
            if (_execute.load()) {
                stop();
            };

        }

        CallBackTimer* CallBackTimer::AsTimerPort() {
            return this;
        }

        const zsock_t *CallBackTimer::GetSocket() const {
            return NULL;
        }

        void CallBackTimer::stop() {
            std::cout << "Timer stopping" << std::endl;
            //_execute.store(false, std::memory_order_release);
            _execute.store(false);
            std::cout << "_execute stored" << std::endl;
            if (_thd.joinable()) {
                std::cout << "joinable" << std::endl;
                _thd.join();
                std::cout << "joined" << std::endl;
            }
            std::cout << "Timer stopped" << std::endl;
        }

        void CallBackTimer::start(int interval) {
            //if (_execute.load(std::memory_order_acquire)) {
            if (_execute.load()) {
                stop();
            };
            //_execute.store(true, std::memory_order_release);
            _execute.store(true);
            _thd = std::thread([this, interval]() {
                //timespec now;
                //clock_gettime(CLOCK_REALTIME, &now);

                auto now = std::chrono::high_resolution_clock::now();
                zsock_t* _zsock_timer = zsock_new_push(_timerresponsechannel.c_str());
                int lingerValue = 0;
                int sendtimeout = 0; // 0 - returns immediately with EAGAIN if the message cannot be sent
                zmq_setsockopt(_zsock_timer, ZMQ_LINGER, &lingerValue, sizeof(int));
                zmq_setsockopt(_zsock_timer, ZMQ_SNDTIMEO, &sendtimeout, sizeof(int));
                //while (_execute.load(std::memory_order_acquire)) {
                while (_execute.load()) {



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

                    //if (_execute.load(std::memory_order_acquire))
                    //if (_execute.load())
                    zmsg_send(&msg, _zsock_timer);


                    //std::this_thread::sleep_for(
                    //        std::chrono::milliseconds(interval));
                }
                zsock_destroy(&_zsock_timer);
            });
        }

        bool CallBackTimer::is_running() const noexcept {
            return (_execute.load() &&

//            return (_execute.load(std::memory_order_acquire) &&
                    _thd.joinable());
        }
    }

}
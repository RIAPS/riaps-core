//
// Created by istvan on 4/20/17.
//

#include <componentmodel/r_oneshottimer.h>

namespace riaps {

    namespace timers {

        OneShotTimer::OneShotTimer(const std::string& timerresponsechannel,
                                   const std::string& timerid,
                                   timespec&      wakeuptime)
                : _execute(false),
                  _timerid(timerid),
                  _wakeuptime(wakeuptime) {

            _zsock_timer = zsock_new_push(timerresponsechannel.c_str());
        }

        OneShotTimer::~OneShotTimer() {
            if (_execute.load(std::memory_order_acquire)) {
                stop();
            };
            zsock_destroy(&_zsock_timer);
        }

        void OneShotTimer::stop() {
            _execute.store(false, std::memory_order_release);
            if (_thd.joinable())
                _thd.join();
        }

        void OneShotTimer::start() {
            if (_execute.load(std::memory_order_acquire)) {
                stop();
            };
            _execute.store(true, std::memory_order_release);
            _thd = std::thread([this]() {
                    // Create the FIRE message, but don't send
                    zmsg_t *msg = zmsg_new();
                    auto portName = this->_timerid;
                    zmsg_addstr(msg, portName.c_str());

                    // Block until _wakeuptime
                    //std::this_thread::sleep_until(std::chrono::system_clock::from_time_t(_wakeuptime));

                    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &_wakeuptime, NULL);
                    zmsg_send(&msg, this->_zsock_timer);
            });
        }

        bool OneShotTimer::is_running() const noexcept {
            return (_execute.load(std::memory_order_acquire) &&
                    _thd.joinable());
        }
    }
}
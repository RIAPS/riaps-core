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
                while (_execute.load(std::memory_order_acquire)) {

                    // Send FIRE message
                    zmsg_t *msg = zmsg_new();
                    zmsg_addstr(msg, GetPortName().c_str());
                    zmsg_send(&msg, this->_zsock_timer);

                    std::this_thread::sleep_for(
                            std::chrono::milliseconds(interval));
                }
            });
        }

        bool CallBackTimer::is_running() const noexcept {
            return (_execute.load(std::memory_order_acquire) &&
                    _thd.joinable());
        }
    }

}
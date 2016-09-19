//
// Created by parallels on 9/19/16.
//

#ifndef RIAPS_FW_R_TIMER_H
#define RIAPS_FW_R_TIMER_H

#include <thread>
#include <functional>
#include <atomic>

namespace riaps {


    class CallBackTimer {
    public:
        CallBackTimer();

        ~CallBackTimer();

        void stop();

        void start(int interval, std::function<void(void)> func);

        bool is_running() const noexcept ;

    protected:
        std::atomic<bool> _execute;
        std::thread _thd;
    };
}

#endif //RIAPS_FW_R_TIMER_H

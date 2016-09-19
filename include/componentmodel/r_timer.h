//
// Created by parallels on 9/19/16.
//

#ifndef RIAPS_FW_R_TIMER_H
#define RIAPS_FW_R_TIMER_H

#include "r_ipc_endpoints.h"

#include <czmq.h>
#include <thread>
#include <functional>
#include <atomic>

namespace riaps {

    class CallBackTimer {
    public:
        CallBackTimer(std::string timerid);

        ~CallBackTimer();

        const zsock_t* GetSocket();

        void stop();

        void start(int interval, std::function<void(void)> func);

        bool is_running() const noexcept ;

    protected:
        std::string       _timerid;
        std::atomic<bool> _execute;
        std::thread       _thd;
        zsock_t*          _zsock_timer;
    };
}

#endif //RIAPS_FW_R_TIMER_H

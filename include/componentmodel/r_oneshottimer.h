//
// Created by istvan on 4/20/17.
//

#ifndef RIAPS_CORE_R_ONESHOTTIMER_H
#define RIAPS_CORE_R_ONESHOTTIMER_H

#include <componentmodel/ports/r_portbase.h>

#include <string>
#include <thread>
#include <functional>
#include <atomic>
#include <ctime>

namespace riaps {

    namespace timers {

        class OneShotTimer {
        public:
            OneShotTimer(const std::string& timerresponsechannel,
                         const std::string& timerid,
                         timespec&      wakeuptime);

            ~OneShotTimer();

            const std::string& GetTimerId() const;

            void stop();
            void start();
            bool is_running() const noexcept;

        protected:
            std::atomic<bool> _execute;
            std::thread       _thd;
            zsock_t*          _zsock_timer;
            std::string       _timerid;
            timespec          _wakeuptime;
        };
    }
}

#endif //RIAPS_CORE_R_ONESHOTTIMER_H

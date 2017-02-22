//
// Created by parallels on 9/19/16.
//

#ifndef RIAPS_FW_R_TIMER_H
#define RIAPS_FW_R_TIMER_H

#include <componentmodel/r_ipc_endpoints.h>
#include <componentmodel/r_portbase.h>

#include <czmq.h>
#include <thread>
#include <functional>
#include <atomic>

namespace riaps {

    namespace ports {

        class CallBackTimer : public PortBase {
        public:
            CallBackTimer(std::string &timerresponsechannel, const _component_port_tim_j& config);

            ~CallBackTimer();

            virtual const zsock_t* GetSocket() const;

            void stop();

            //void start(int interval, std::function<void(void)> func);
            void start(int interval);

            bool is_running() const noexcept;

            virtual CallBackTimer*  AsTimerPort();

        protected:
            std::atomic<bool> _execute;
            std::thread _thd;
            zsock_t *_zsock_timer;
        };
    }
}

#endif //RIAPS_FW_R_TIMER_H

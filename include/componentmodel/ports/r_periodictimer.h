#ifndef RIAPS_CORE_R_PERIODIC_TIMER_H
#define RIAPS_CORE_R_PERIODIC_TIMER_H

#include <componentmodel/ports/r_portbase.h>
#include <componentmodel/ports/r_recvport.h>
#include <czmq.h>
#include <string>
#include <functional>

namespace riaps {
    namespace ports {

        void ptimeractor(zsock_t* pipe, void* args);

        class PeriodicTimer : public PortBase {
        public:
            PeriodicTimer(const ComponentPortTim& config, const ComponentBase* parent_component);

            void Init();
            std::string Recv();

            int interval();
            void Stop();
            void Start();

            bool is_running() const noexcept;
            virtual const zsock_t* port_socket() const;


            //virtual PeriodicTimer* AsTimerPort() override;
            std::string TimerChannel();

            virtual ~PeriodicTimer() = default;
        protected:
            std::string             timer_channel_;
            zactor_t*               timer_actor_;
            ulong                   interval_;
            std::unique_ptr<zmsg_t, std::function<void(zmsg_t*)>> last_zmsg_;
        };
    }
}

#endif //RIAPS_CORE_R_PERIODIC_TIMER_H

#ifndef RIAPS_CORE_R_PERIODIC_TIMER_H
#define RIAPS_CORE_R_PERIODIC_TIMER_H

#include <componentmodel/ports/r_portbase.h>
#include <czmq.h>
#include <string>
#include <functional>

namespace riaps {
    namespace ports {

        /**
         * The timer thread.
         * @param pipe ZMQ pair socket, accepts/sends messages between the caller and the timer thread.
         * @param args Arguments for the timer thread.
         */
        void ptimeractor(zsock_t* pipe, void* args);

        /**
         * Holds the timer parameters, initializes the timer thread (ZMQ zactor).
         */
        class PeriodicTimer : public PortBase {
            friend void ptimeractor(zsock_t* pipe, void* args);
        public:
            /**
             * Instantiates a new timer object and starts the timer thread.
             * @param config
             * @param parent_component
             */
            PeriodicTimer(const ComponentPortTim& config, const ComponentBase* parent_component);

            /**
             * Creates a ZMQ PULL socket, events from the timer thread can be read from this port.
             */
            void Init();

            /**
             * Reads the fired timestamp from the ZMQ socket.
             * @return
             */
            timespec Recv();

            /**
             * \defgroup PAPI Port API
             * @{
             */


            ulong interval();
            void Stop();
            void Start();
            void Halt();
            void delay(timespec& value);
            const timespec delay();

            bool has_delay();

            virtual const zsock_t* port_socket() const;

            std::string TimerChannel();

            ~PeriodicTimer() override = default;
            ///@}
        private:
            std::string             timer_channel_;
            zactor_t*               timer_actor_;
            ulong                   interval_;
            timespec                delay_;
            timespec                now_;
            bool                    has_started_;
            std::mutex              mtx_;
            std::unique_ptr<zmsg_t, std::function<void(zmsg_t*)>> last_zmsg_;
        };
    }
}

#endif //RIAPS_CORE_R_PERIODIC_TIMER_H

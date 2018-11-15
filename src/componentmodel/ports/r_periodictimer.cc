//
// Created by istvan on 5/2/17.
//

#include <componentmodel/ports/r_periodictimer.h>

#include <chrono>
#include <thread>
#include <functional>
#include <atomic>

using namespace std;

namespace riaps {
    namespace ports {

        void ptimeractor(zsock_t* pipe, void* args){
            PeriodicTimer* periodic_timer = (PeriodicTimer*)args;

            zpoller_t* poller = zpoller_new(pipe, NULL);
            assert(poller);

            zpoller_set_nonstop(poller, true);

            zsock_signal (pipe, 0);

            bool terminated = false;
            bool started    = false;

            zsock_t* zsock_timer = zsock_new_push(periodic_timer->TimerChannel().c_str());
            int linger_value = 0;
            int send_timeout = 0; // 0 - returns immediately with EAGAIN if the message cannot be sent
            zmq_setsockopt(zsock_timer, ZMQ_LINGER,   &linger_value, sizeof(int));
            zmq_setsockopt(zsock_timer, ZMQ_SNDTIMEO, &send_timeout, sizeof(int));

            auto now = chrono::high_resolution_clock::now();

            while (!terminated) {

                if (started) {
                    // Send FIRE message
                    zmsg_t *msg = zmsg_new();
                    auto port_name = periodic_timer->GetPortName();
                    zmsg_addstr(msg, port_name.c_str());

                    auto diff = chrono::milliseconds(periodic_timer->interval());
                    now += diff;
                    this_thread::sleep_until(now);
                    zmsg_send(&msg, zsock_timer);
                } else{
                    this_thread::sleep_for(chrono::milliseconds(10));
                }

                void *which = zpoller_wait(poller, 0);
                if (which == pipe) {
                    zmsg_t *msg = zmsg_recv(which);
                    if (msg == nullptr) {
                        std::cout << "No msg => interrupted" << std::endl;
                        break;
                    }

                    char *command = zmsg_popstr(msg);

                    if (streq(command, "$TERM")) {
                        cout << "$TERM arrived in periodic timer" << std::endl;
                        terminated = true;
                    } else if (streq(command, "$START")){
                        started = true;
                        now = chrono::high_resolution_clock::now();
                    }
                    zstr_free(&command);
                    zmsg_destroy(&msg);
                }
            }
            zsock_destroy(&zsock_timer);
            zpoller_destroy(&poller);
        }


        PeriodicTimer::PeriodicTimer(const component_port_tim &config, const ComponentBase* parent_component)
                : PortBase(PortTypes::Timer, (component_port_config*)&config, parent_component) {
            interval_ = config.period;
            timer_actor_ = zactor_new(ptimeractor, this);
        }

        void PeriodicTimer::Init() {
            port_socket_ = zsock_new_pull(TimerChannel().c_str());
        }

        string PeriodicTimer::Recv() {
            zmsg_t *msg = zmsg_recv(const_cast<zsock_t*>(GetSocket()));
            if (msg) {
                last_zmsg_ = unique_ptr<zmsg_t, function<void(zmsg_t*)>>(msg, [](zmsg_t *z) { zmsg_destroy(&z); });
                auto str = zmsg_popstr(last_zmsg_.get());
                return str;
            }
            return "";
        }

        string PeriodicTimer::TimerChannel() {
            return fmt::format("inproc://timer_{}", GetPortName());
        }

        int PeriodicTimer::interval() {
            return interval_;
        }

        void PeriodicTimer::Start() {
            zmsg_t* msg = zmsg_new();
            zmsg_addstr(msg,"$START");
            zactor_send(timer_actor_, &msg);
        }

        void PeriodicTimer::Stop() {
            zactor_destroy(&timer_actor_);
        }

        const zsock_t* PeriodicTimer::GetSocket() const {
            return port_socket_;
        }

        PeriodicTimer* PeriodicTimer::AsTimerPort() {
            return this;
        }
    }
}
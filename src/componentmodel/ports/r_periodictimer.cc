//
// Created by istvan on 5/2/17.
//

#include <componentmodel/ports/r_periodictimer.h>

#include <chrono>
#include <thread>
#include <functional>
#include <atomic>
#include <mutex>

using namespace std;

constexpr auto CMD_START = "$START";
constexpr auto CMD_HALT  = "$HALT" ;

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
            int send_timeout = 0;
            zmq_setsockopt(zsock_timer, ZMQ_LINGER,   &linger_value, sizeof(int));
            zmq_setsockopt(zsock_timer, ZMQ_SNDTIMEO, &send_timeout, sizeof(int));

            //auto now = chrono::high_resolution_clock::now();
            timespec& now = periodic_timer->now_;
            clock_gettime(CLOCK_REALTIME, &now);

            timespec period = {
                        periodic_timer->interval()/1000,
                        (periodic_timer->interval()%1000)*1000000
                    };


            while (!terminated) {
                if (started) {
                    if (periodic_timer->has_delay()) {
                        period = periodic_timer->delay();
                        if (period.tv_sec == 0 && period.tv_nsec == 0) continue;

                        clock_gettime(CLOCK_REALTIME, &now);
                    }

                    now.tv_sec+=period.tv_sec;
                    if (now.tv_nsec + period.tv_nsec >= 1E9) {
                        now.tv_nsec = now.tv_nsec + period.tv_nsec - 1E9;
                        now.tv_sec++;
                    }
                    else
                        now.tv_nsec+=period.tv_nsec;

                    zframe_t *msg = zframe_new(&now, sizeof(timespec));
                    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &now, NULL);
                    zframe_send(&msg, zsock_timer, ZFRAME_DONTWAIT);
                    if (periodic_timer->has_delay()) {
                        periodic_timer->Halt();
                    }
                } else {
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
                    } else if (streq(command, CMD_START)){
                        started = true;
                        clock_gettime(CLOCK_REALTIME, &now);
                    } else if (streq(command, CMD_HALT)){
                        started = false;
                    }
                    zstr_free(&command);
                    zmsg_destroy(&msg);
                }
            }
            zsock_destroy(&zsock_timer);
            zpoller_destroy(&poller);
        }


        PeriodicTimer::PeriodicTimer(const ComponentPortTim &config, const ComponentBase* parent_component)
                : PortBase(PortTypes::Timer, (ComponentPortConfig*)&config, parent_component) {
            interval_    = config.period;
            delay_       = timespec{0,0};
            has_started_ = false;
            timer_actor_ = zactor_new(ptimeractor, this);
        }

        void PeriodicTimer::Init() {
            port_socket_ = zsock_new_pull(TimerChannel().c_str());
        }

        timespec PeriodicTimer::Recv() {
            timespec result {0,0};
            zframe_t* msg = zframe_recv(const_cast<zsock_t*>(port_socket()));
            if (msg) {
                unsigned char* data = zframe_data(msg);
                memcpy(&result, data, sizeof(timespec));
                zframe_destroy(&msg);
            }
            return result;
//            zmsg_t *msg = zmsg_recv(const_cast<zsock_t*>(port_socket()));
//            if (msg) {
//                last_zmsg_ = unique_ptr<zmsg_t, function<void(zmsg_t*)>>(msg, [](zmsg_t *z) { zmsg_destroy(&z); });
//                auto str = zmsg_popstr(last_zmsg_.get());
//                return str;
//            }
//            return "";
        }

        string PeriodicTimer::TimerChannel() {
            return fmt::format("inproc://timer_{}", port_name());
        }

        ulong PeriodicTimer::interval() {
            return interval_;
        }

        void PeriodicTimer::Start() {
            if (!has_started_) {
                has_started_=true;
                clock_gettime(CLOCK_REALTIME, &now_);
                zmsg_t* msg = zmsg_new();
                zmsg_addstr(msg, CMD_START);
                zactor_send(timer_actor_, &msg);
            }
        }

        bool PeriodicTimer::has_delay() {
            //return !(delay_.tv_sec == 0 && delay_.tv_nsec == 0);
            return interval_ == 0;
        }

        void PeriodicTimer::Halt() {
            if (has_started_) {
                has_started_ = false;
                zmsg_t *msg = zmsg_new();
                zmsg_addstr(msg, CMD_HALT);
                zactor_send(timer_actor_, &msg);
            }
        }

        const timespec PeriodicTimer::delay() {
            mtx_.lock();
            auto result = delay_;
            mtx_.unlock();
            return result;

        }

        void PeriodicTimer::delay(timespec &value) {
            mtx_.lock();
            delay_ = value;
            mtx_.unlock();
        }

        void PeriodicTimer::Stop() {
            zactor_destroy(&timer_actor_);
        }

        const zsock_t* PeriodicTimer::port_socket() const {
            return port_socket_;
        }
    }
}
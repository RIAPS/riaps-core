
#ifndef RIAPS_CORE_R_TIMEOUT_H
#define RIAPS_CORE_R_TIMEOUT_H

#include <chrono>

using namespace std::chrono;

namespace riaps{
    namespace utils{
        template<typename T>
        class Timeout {
        public:

            /**
             * Intitalizes with 0 timeout from now;
             */

            Timeout<T>(){
                Reset(duration<int, T>(0));
            };

            /**
             * Initializes the timeut structure from ::now() with the passed timeout
             * @param timeout
             */
            Timeout<T>(duration<int, T> timeout){
                Reset(timeout);
            };

            Timeout<T>(int timeout) {
                std::chrono::duration<int,T> d(timeout);
                Reset(d);
            }

            /**
             * Resets the start time point, doesn't touch the timeout
             */
            void Reset() {
                m_startPoint = steady_clock::now();
                m_endPoint = m_startPoint + m_timeout;
            };

            /**
             * Resets the start point and the timeout value
             * @param timeout
             */
            void Reset(duration<int, T> timeout){
                m_timeout = timeout;
                Reset();
            };

            /**
             * If ::now()>_endPoint
             * @return
             */
            bool IsTimeout() {
                auto now = steady_clock::now();
                return now > m_endPoint;
            }

            steady_clock::time_point GetEndTimePoint() {
                return m_endPoint;
            }

            ~Timeout() = default;
        private:
            steady_clock::time_point  m_startPoint; // The election timeout from this timepoint
            duration<int, T> m_timeout; // The election timeout
            steady_clock::time_point  m_endPoint;
        };
    }
}

#endif //RIAPS_CORE_R_TIMEOUT_H

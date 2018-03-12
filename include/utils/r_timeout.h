
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

            /**
             * Resets the start time point, doesn't touch the timeout
             */
            void Reset() {
                _startPoint = steady_clock::now();
                _endPoint = _startPoint + _timeout;
            };

            /**
             * Resets the start point and the timeout value
             * @param timeout
             */
            void Reset(duration<int, T> timeout){
                _timeout = timeout;
                Reset();
            };

            /**
             * If ::now()>_endPoint
             * @return
             */
            bool IsTimeout() {
                auto now = steady_clock::now();
                return now > _endPoint;
            }

            steady_clock::time_point GetEndTimePoint() {
                return _endPoint;
            }

            ~Timeout() = default;
        private:
            steady_clock::time_point  _startPoint; // The election timeout from this timepoint
            duration<int, T> _timeout; // The election timeout
            steady_clock::time_point  _endPoint;
        };
    }
}

#endif //RIAPS_CORE_R_TIMEOUT_H

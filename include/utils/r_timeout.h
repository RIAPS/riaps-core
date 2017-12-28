
#ifndef RIAPS_CORE_R_TIMEOUT_H
#define RIAPS_CORE_R_TIMEOUT_H

#include <chrono>

using namespace std::chrono;

namespace riaps{
    namespace utils{
        class Timeout {
        public:

            /**
             * Intitalizes with 0 timeout from now;
             */
            Timeout();

            /**
             * Initializes the timeut structure from ::now() with the passed timeout
             * @param timeout
             */
            Timeout(duration<int, std::milli> timeout);

            /**
             * Resets the start time point, doesn't touch the timeout
             */
            void Reset();

            /**
             * Resets the start point and the timeout value
             * @param timeout
             */
            void Reset(duration<int, std::milli> timeout);

            /**
             * If ::now()>_endPoint
             * @return
             */
            bool IsTimeout();

            steady_clock::time_point GetEndTimePoint();

            ~Timeout();
        private:
            steady_clock::time_point  _startPoint; // The election timeout from this timepoint
            duration<int, std::milli> _timeout; // The election timeout
            steady_clock::time_point  _endPoint;
        };
    }
}

#endif //RIAPS_CORE_R_TIMEOUT_H

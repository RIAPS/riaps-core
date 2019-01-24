
#ifndef RIAPS_CORE_R_TIMEOUT_H
#define RIAPS_CORE_R_TIMEOUT_H

#include <chrono>
#include <random>

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
                Reset(T(0));
            };

            /**
             * Initializes the timeut structure from ::now() with the passed timeout
             * @param timeout
             */
            Timeout<T>(T timeout){
                Reset(timeout);
            };

            Timeout<T>(int64_t timeout) {
                T d(timeout);
                Reset(d);
            }

            Timeout<T>(int64_t lower_bound, int64_t upper_bound) {
                Reset(lower_bound, upper_bound);
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
            void Reset(T timeout){
                m_timeout = timeout;
                Reset();
            };

            void Reset(int64_t timeout) {
                m_timeout = T(timeout);
            }

            void Reset(int64_t lower_bound, int64_t upper_bound) {
                std::random_device                      rd;
                std::mt19937                            gen(rd());
                std::uniform_int_distribution<int64_t > dist(lower_bound, upper_bound);
                int64_t v = dist(gen);
                Reset(v);
            }

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
            T m_timeout; // The election timeout
            steady_clock::time_point  m_endPoint;

//            std::random_device                      rd;
//            std::mt19937                            gen;
//            std::uniform_int_distribution<int64_t > dist;
        };
    }
}

//namespace riaps{
//    namespace utils{
//        template<typename T>
//        class Timeout {
//        public:
//
//            /**
//             * Intitalizes with 0 timeout from now;
//             */
//
//            Timeout<T>(){
//                Reset(duration<int64_t , T>(0));
//            };
//
//            /**
//             * Initializes the timeut structure from ::now() with the passed timeout
//             * @param timeout
//             */
//            Timeout<T>(duration<int64_t, T> timeout){
//                Reset(timeout);
//            };
//
//            Timeout<T>(int64_t timeout) {
//                std::chrono::duration<int64_t,T> d(timeout);
//                Reset(d);
//            }
//
//            /**
//             * Resets the start time point, doesn't touch the timeout
//             */
//            void Reset() {
//                m_startPoint = steady_clock::now();
//                m_endPoint = m_startPoint + m_timeout;
//            };
//
//            /**
//             * Resets the start point and the timeout value
//             * @param timeout
//             */
//            void Reset(duration<int64_t, T> timeout){
//                m_timeout = timeout;
//                Reset();
//            };
//
//            /**
//             * If ::now()>_endPoint
//             * @return
//             */
//            bool IsTimeout() {
//                auto now = steady_clock::now();
//                return now > m_endPoint;
//            }
//
//            steady_clock::time_point GetEndTimePoint() {
//                return m_endPoint;
//            }
//
//            ~Timeout() = default;
//        private:
//            steady_clock::time_point  m_startPoint; // The election timeout from this timepoint
//            duration<int64_t, T> m_timeout; // The election timeout
//            steady_clock::time_point  m_endPoint;
//        };
//    }
//}

#endif //RIAPS_CORE_R_TIMEOUT_H

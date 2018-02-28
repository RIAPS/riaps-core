#include <utils/r_timeout.h>

namespace riaps {
    namespace utils {


        Timeout::Timeout() {
            Reset(duration<int, std::milli>(0));
        }

        Timeout::Timeout(duration<int, std::milli> timeout) {
            Reset(timeout);
        }

        void Timeout::Reset() {
            _startPoint = steady_clock::now();
            _endPoint = _startPoint + _timeout;
        }

        void Timeout::Reset(duration<int, std::milli> timeout) {
            _timeout = timeout;
            Reset();
        }

        steady_clock::time_point Timeout::GetEndTimePoint() {
            return _endPoint;
        }

        bool Timeout::IsTimeout() {
            auto now = steady_clock::now();
            return now > _endPoint;
        }

        Timeout::~Timeout() = default;
    }
}
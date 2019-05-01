//
// Created by istvan on 11/28/18.
//

#ifndef RIAPS_CORE_R_PORTERROR_H
#define RIAPS_CORE_R_PORTERROR_H
#include <czmq.h>

namespace riaps {
    namespace ports {

        enum ZmqErrors {
            kEAGAIN       = EAGAIN       ,
            kENOTSUP      = ENOTSUP      ,
            kEFSM         = EFSM         ,
            kETERM        = ETERM        ,
            kENOTSOCK     = ENOTSOCK     ,
            kEINTR        = EINTR        ,
            kEFAULT       = EFAULT       ,
            kEHOSTUNREACH = EHOSTUNREACH   // Only in Send()
        };

        class PortError {
        public:
            explicit PortError(bool success, int error_code=0);
            ~PortError() = default;

            /// True if error has occurred in the last operation.
            bool      error     () const;

            /// True if no error has occurred in the last operation.
            bool      success   () const;

            /// The ZMQ error code.
            int       error_code() const;

            /**
             * bool operator override, so the object can be used in if.
             * @return True if error has occured in the last operation.
             */
            operator bool() const {
                return error();
            };

        private:
            bool      success_;
            int       error_code_;
            ZmqErrors zmq_error_;
        };

        inline PortError::PortError(bool success, int error_code) : success_(success), error_code_(error_code) {
            if (!this->success()) {
                zmq_error_ = static_cast<ZmqErrors >(error_code);
            }
        }

        inline bool PortError::success() const {
            return success_;
        }

        inline bool PortError::error() const {
            return !success_;
        }

        inline int PortError::error_code() const {
            return error_code_;
        }
    }
}

#endif //RIAPS_CORE_R_PORTERROR_H

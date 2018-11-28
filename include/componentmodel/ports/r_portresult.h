//
// Created by istvan on 11/28/18.
//

#ifndef RIAPS_CORE_R_PORTRESULT_H
#define RIAPS_CORE_R_PORTRESULT_H
#include <czmq.h>


namespace riaps {
    namespace ports {

        class PortResult {
        public:
            PortResult(int czmq_rc);
            ~PortResult() = default;

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

            bool      success  () const;
            ZmqErrors zmq_error() const;

            operator bool() const {
                return success();
            };


        private:
            ZmqErrors zmq_error_;
            int       zmq_rc_;
        };
    }
}

#endif //RIAPS_CORE_R_PORTRESULT_H

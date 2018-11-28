//
// Created by istvan on 11/28/18.
//

#include <componentmodel/ports/r_portresult.h>

namespace riaps {
    namespace ports {

        PortResult::PortResult(int czmq_rc):zmq_rc_(czmq_rc) {
            if (!success()) {
                zmq_error_ = static_cast<ZmqErrors >(zmq_errno());
            }
        }

        inline bool PortResult::success() const {
            return zmq_rc_ == 0;
        }

        inline PortResult::ZmqErrors PortResult::zmq_error() const {
            return zmq_error_;
        }

    }
}
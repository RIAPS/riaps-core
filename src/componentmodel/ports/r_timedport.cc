//
// Created by istvan on 3/29/18.
//

#include <componentmodel/ports/r_timedport.h>

namespace riaps {
    namespace ports {
        TimedPort::TimedPort() {
            sent_timestamp_.tv_nsec = sent_timestamp_.tv_sec  = 0;
            recv_timestamp_.tv_sec  = recv_timestamp_.tv_nsec = 0;
        }

        const timespec TimedPort::LastSendTimestamp() {
            return sent_timestamp_;
        }

        const timespec TimedPort::LastRecvTimestamp() {
            return recv_timestamp_;
        }

        void TimedPort::SetRecvTimestamp(timespec recv_timestamp) {
            recv_timestamp_ = recv_timestamp;
        }

        void TimedPort::SetSendTimestamp(timespec sent_timestamp) {
            sent_timestamp_ = sent_timestamp;
        }

        void TimedPort::SetSendTimestamp(double sent_timestamp) {
            timespec t;
            t.tv_sec = sent_timestamp;
            t.tv_nsec = (sent_timestamp - t.tv_sec)*1000000000.0;
            sent_timestamp_ = t;
        }

        void TimedPort::SetRecvTimestamp(double recv_timestamp) {
            timespec t;
            t.tv_sec = recv_timestamp;
            t.tv_nsec = (recv_timestamp-t.tv_sec)*1000000000.0;
            recv_timestamp_ = t;
        }
    }
}
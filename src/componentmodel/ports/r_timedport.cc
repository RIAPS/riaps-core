//
// Created by istvan on 3/29/18.
//

#include <componentmodel/ports/r_timedport.h>

namespace riaps {
    namespace ports {
        TimedPort::TimedPort() {
            m_sentTimestamp.tv_nsec = m_sentTimestamp.tv_sec  = 0;
            m_recvTimestamp.tv_sec  = m_recvTimestamp.tv_nsec = 0;
        }
        const timespec TimedPort::GetLastSendTimestamp() {return m_sentTimestamp; }
        const timespec TimedPort::GetLastRecvTimestamp() {return m_recvTimestamp; }
        void TimedPort::SetRecvTimestamp(timespec t_recvTimestamp) {m_recvTimestamp = t_recvTimestamp;}
        void TimedPort::SetSendTimestamp(timespec t_sentTimestamp) {m_sentTimestamp = t_sentTimestamp;}
        void TimedPort::SetSendTimestamp(double t_sentTimestamp) {
            timespec t;
            t.tv_sec = t_sentTimestamp;
            t.tv_nsec = (t_sentTimestamp-t.tv_nsec)*1000000000.0;
            m_sentTimestamp = t;
        }

        void TimedPort::SetRecvTimestamp(double t_recvTimestamp) {
            timespec t;
            t.tv_sec = t_recvTimestamp;
            t.tv_nsec = (t_recvTimestamp-t.tv_nsec)*1000000000.0;
            m_recvTimestamp = t;
        }


    }
}
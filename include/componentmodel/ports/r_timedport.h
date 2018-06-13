//
// Created by istvan on 3/29/18.
//

#ifndef RIAPS_CORE_R_TIMEDPORT_H
#define RIAPS_CORE_R_TIMEDPORT_H

#include <ctime>

namespace riaps {
    namespace ports{
        class TimedPort {
        public:
            TimedPort();
            ~TimedPort() = default;

            const timespec GetLastRecvTimestamp();
            const timespec GetLastSendTimestamp();




        protected:
            void SetRecvTimestamp(double t_recvTimestamp);
            void SetSendTimestamp(double t_sentTimestamp);

            void SetRecvTimestamp(timespec t_recvTimestamp);
            void SetSendTimestamp(timespec t_sentTimestamp);



        private:
            timespec m_recvTimestamp;
            timespec m_sentTimestamp;
        };
    }
}




#endif //RIAPS_CORE_R_TIMEDPORT_H

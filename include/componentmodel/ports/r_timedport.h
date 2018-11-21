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
            const timespec LastRecvTimestamp();
            const timespec LastSendTimestamp();

        protected:
            void SetRecvTimestamp(double recv_timestamp);
            void SetSendTimestamp(double sent_timestamp);

            void SetRecvTimestamp(timespec recv_timestamp);
            void SetSendTimestamp(timespec sent_timestamp);

        private:
            timespec recv_timestamp_;
            timespec sent_timestamp_;
        };
    }
}




#endif //RIAPS_CORE_R_TIMEDPORT_H

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
            virtual ~TimedPort() = default;
            const timespec recv_timestamp();
            const timespec send_timestamp();

        protected:
            void set_recv_timestamp(double recv_timestamp);
            void set_send_timestamp(double sent_timestamp);

            void set_recv_timestamp(timespec recv_timestamp);
            void set_send_timestamp(timespec sent_timestamp);

        private:
            timespec recv_timestamp_;
            timespec sent_timestamp_;
        };
    }
}




#endif //RIAPS_CORE_R_TIMEDPORT_H

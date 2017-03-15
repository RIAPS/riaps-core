//
// Created by istvan on 3/15/17.
//

#ifndef RIAPS_CORE_SENSORQUERY_H
#define RIAPS_CORE_SENSORQUERY_H

#include <msgpack.hpp>

#include <string>

namespace distributedestimator{
    namespace messages{
        class SensorQuery{
        public:
            SensorQuery();

            void SetMsg(const std::string& msg);
            const std::string& GetMsg();

            MSGPACK_DEFINE(_msg);

            ~SensorQuery();
        private:
            std::string _msg;

        };
    }
}



#endif //RIAPS_CORE_SENSORQUERY_H

//
// Created by istvan on 3/14/17.
//

#ifndef RIAPS_CORE_SENSORREADY_H
#define RIAPS_CORE_SENSORREADY_H

#include <vector>
#include <msgpack.hpp>

//#include <messaging/r_message.h>

//class SensorReady : public IRiapsMessage<std::string>
//{
//    SensorReady();
//
//    zmsg_t* SerializeMessage(std::string message);
//    byte*   SerializeMessage(std::string message);
//
//    std::string DeserializeMessage(zmsg_t* message);
//    std::string DeserializeMessage(byte* message);
//
//
//    ~SensorReady();
//};

namespace distributedestimator {
    namespace messages {

        class SensorReady {
        public:
            SensorReady();

            void SetMsg(const std::string &msg);

            const std::string &GetMsg();

            MSGPACK_DEFINE (_msg);

            ~SensorReady();

        private:
            std::string _msg;

        };
    }
}

#endif //RIAPS_CORE_SENSORREADY_H

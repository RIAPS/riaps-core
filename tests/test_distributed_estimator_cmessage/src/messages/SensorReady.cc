//
// Created by istvan on 3/14/17.
//

#include <messages/SensorReady.h>

namespace distributedestimator {
    namespace messages {
        SensorReady::SensorReady() {

        }

        void SensorReady::SetMsg(const std::string &msg) {
            _msg = msg;
        }

        const std::string &SensorReady::GetMsg() {
            return _msg;
        }

        SensorReady::~SensorReady() {

        }
    }
}

//std::string SensorReady::DeserializeMessage(zmsg_t *message) {
//
//}
//
//zmsg_t* SensorReady::SerializeMessage(std::string message) {
//
//}
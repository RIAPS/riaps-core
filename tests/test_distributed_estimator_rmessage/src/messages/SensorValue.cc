//
// Created by istvan on 3/15/17.
//

#include <messages/SensorValue.h>

namespace distributedestimator {
    namespace messages {
        SensorValue::SensorValue() {

        }

        void SensorValue::SetMsg(const std::string &msg) {
            _msg = msg;
        }

        const std::string &SensorValue::GetMsg() {
            return _msg;
        }

        SensorValue::~SensorValue() {

        }
    }
}
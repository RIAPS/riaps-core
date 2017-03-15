//
// Created by istvan on 3/15/17.
//

#include <messages/SensorQuery.h>

namespace distributedestimator {
    namespace messages {

        SensorQuery::SensorQuery() {
        }

        const std::string &SensorQuery::GetMsg() {
            return _msg;
        }

        void SensorQuery::SetMsg(const std::string &msg) {
            _msg = msg;
        }

        SensorQuery::~SensorQuery() {}
    }
}
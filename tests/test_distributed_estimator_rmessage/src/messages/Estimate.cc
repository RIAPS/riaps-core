//
// Created by istvan on 3/15/17.
//

#include "messages/Estimate.h"

namespace distributedestimator {
    namespace messages {

        Estimate::Estimate() {
        }

        const std::string &Estimate::GetMsg() {
            return _msg;
        }

        void Estimate::SetMsg(const std::string &msg) {
            _msg = msg;
        }

        Estimate::~Estimate() {}
    }
}
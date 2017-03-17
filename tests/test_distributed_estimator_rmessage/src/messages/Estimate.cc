//
// Created by istvan on 3/15/17.
//

#include "messages/Estimate.h"

namespace distributedestimator {
    namespace messages {

        std::string EmB::GetLofasz() {
            return _lofasz;
        }

        void EmB::SetLofasz(const std::string &message) {
            _lofasz = message;
        }

        Estimate::Estimate() {
        }

        const std::string &Estimate::GetMsg() {
            return _msg;
        }

        void Estimate::SetMsg(const std::string &msg) {
            _msg = msg;
        }

        std::vector<double>& Estimate::GetData() {
            return _data;
        }

        Estimate::~Estimate() {}
    }
}
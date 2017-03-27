//
// Created by istvan on 3/15/17.
//

#ifndef RIAPS_CORE_SENSORVALUE_H
#define RIAPS_CORE_SENSORVALUE_H

#include "capnp/distributedestimator.capnp.h"
#include <componentmodel/r_messagebase.h>

#include <string>

namespace distributedestimator{
    namespace messages{
        class SensorValue : public riaps::MessageBase {
        public:
            SensorValue();
            //virtual void InitFields();

            void SetMsg(const std::string &msg);

            const std::string GetMsg();

            //virtual kj::ArrayPtr<const kj::ArrayPtr<const capnp::word>> GetBytes();

            ~SensorValue();

        private:
            ::SensorValue::Builder _builder;
            ::SensorValue::Reader  _reader;

        };
    }
}

#endif //RIAPS_CORE_SENSORVALUE_H

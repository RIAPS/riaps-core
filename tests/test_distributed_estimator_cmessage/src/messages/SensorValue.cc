//
// Created by istvan on 3/15/17.
//

#include <messages/SensorValue.h>

namespace distributedestimator {
    namespace messages {
        SensorValue::SensorValue() :
                _builder(_message.getRoot<::SensorValue>()) {

        }

//        void SensorValue::InitFields() {
//
//        }

        void SensorValue::SetMsg(const std::string &msg) {
            _builder.setMsg(msg);
        }

//        kj::ArrayPtr<const kj::ArrayPtr<const capnp::word>> SensorValue::GetBytes() {
//            return _message.getSegmentsForOutput();
//        }

        const std::string SensorValue::GetMsg() {

            return _builder.asReader().getMsg();
        }

        SensorValue::~SensorValue() {

        }
    }
}
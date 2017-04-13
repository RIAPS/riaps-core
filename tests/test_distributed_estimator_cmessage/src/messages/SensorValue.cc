//
// Created by istvan on 3/15/17.
//

#include <messages/SensorValue.h>
#include <stdexcept>

namespace distributedestimator {
    namespace messages {
        SensorValue::SensorValue() :
                _builder(_message.initRoot<::SensorValue>()) {

        }

        void SensorValue::InitReader(capnp::FlatArrayMessageReader* flatArrayMessageReader) {
            SetAsReader();
            _reader= flatArrayMessageReader->getRoot<::SensorValue>();
        }

//        void SensorValue::InitFields() {
//
//        }

        void SensorValue::SetMsg(const std::string &msg) {
            if (IsBuilder()) {
                _builder.setMsg(msg);
            }
            else{
                throw std::runtime_error("Calling setter is invalid on reader type objects.");
            }
        }

//        kj::ArrayPtr<const kj::ArrayPtr<const capnp::word>> SensorValue::GetBytes() {
//            return _message.getSegmentsForOutput();
//        }

        const std::string SensorValue::GetMsg() {

            if (IsBuilder())
                return _builder.asReader().getMsg();
            return _reader.getMsg();
        }

        SensorValue::~SensorValue() {

        }
    }
}
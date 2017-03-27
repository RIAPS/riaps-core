//
// Created by istvan on 3/14/17.
//

#include <messages/SensorReady.h>

namespace distributedestimator {
    namespace messages {
        SensorReady::SensorReady() :
                _builder(nullptr)
        {
            _builder = _message.getRoot<::SensorReady>();
        }

        SensorReady::SensorReady(kj::ArrayPtr<const capnp::word>& rawMessage) :
                MessageBase(rawMessage),
                _builder(nullptr)
        {

        }

       // void SensorReady::InitFields() {
            // Init all fields.
            // Simply types, like strings, ints etc shouldn be initialized
            // Including the complex types and all fields of the complex types


        //}

//        kj::ArrayPtr<const kj::ArrayPtr<const capnp::word>> SensorReady::GetBytes() {
//           return _message.getSegmentsForOutput();
//        }

        void SensorReady::SetMsg(const std::string &msg) {

            if (IsBuilder()) {
                _builder.setMsg(msg);
            }
            else{
                throw std::runtime_error("Calling setter is invalid on reader type objects.");
            }
        }

        const std::string SensorReady::GetMsg() {
            if (IsBuilder())
                return _builder.asReader().getMsg();
            return _reader.getMsg();
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
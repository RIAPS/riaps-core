//
// Created by istvan on 3/15/17.
//

#include <messages/SensorQuery.h>
#include <stdexcept>

namespace distributedestimator {
    namespace messages {

        SensorQuery::SensorQuery() :
                _builder(_message.getRoot<::SensorQuery>()) {
        }

        //void SensorQuery::InitFields() {

        //}

        void SensorQuery::SetMsg(const std::string &msg) {

            if (IsBuilder()) {
                _builder.setMsg(msg);
            }
            else{
                throw std::runtime_error("Calling setter is invalid on reader type objects.");
            }
        }

        const std::string SensorQuery::GetMsg() {
            if (IsBuilder())
                return _builder.asReader().getMsg();
            return _reader.getMsg();
        }

        SensorQuery::~SensorQuery() {}
    }
}
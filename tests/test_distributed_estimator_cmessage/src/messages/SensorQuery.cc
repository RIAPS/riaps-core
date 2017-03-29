//
// Created by istvan on 3/15/17.
//

#include <messages/SensorQuery.h>
#include <stdexcept>

namespace distributedestimator {
    namespace messages {

        SensorQuery::SensorQuery() :
                _builder(_message.initRoot<::SensorQuery>()) {
        }

        void SensorQuery::InitReader(capnp::FlatArrayMessageReader* flatArrayMessageReader) {
            SetAsReader();
            _reader= flatArrayMessageReader->getRoot<::SensorQuery>();
        }


        void SensorQuery::SetMsg(const std::string &msg) {

            if (IsBuilder()) {
                _builder.setMsg(msg);
            }
            else{
                throw std::runtime_error("Calling setter is invalid on reader type objects.");
            }
        }

        const std::string SensorQuery::GetMsg() const {
            if (IsBuilder())
                return _builder.asReader().getMsg();
            return _reader.getMsg();
        }

        SensorQuery::~SensorQuery() {}
    }
}
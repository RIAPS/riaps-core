//
// Created by istvan on 3/15/17.
//

#include "messages/Estimate.h"

#include <stdexcept>

namespace distributedestimator {
    namespace messages {

        Estimate::Estimate()
            : _builder(_message.initRoot<::Estimate>())
        {
        }

        void Estimate::InitReader(capnp::FlatArrayMessageReader* flatArrayMessageReader) {
            SetAsReader();
            _reader= flatArrayMessageReader->getRoot<::Estimate>();
        }

        void Estimate::SetMsg(const std::string &msg) {

            if (IsBuilder()) {
                _builder.setMsg(msg);
            }
            else{
                throw std::runtime_error("Calling setter is invalid on reader type objects.");
            }
        }

        const std::string Estimate::GetMsg() {
            if (IsBuilder())
                return _builder.asReader().getMsg();
            return _reader.getMsg();
        }

        std::vector<double>& Estimate::GetData() {

        }

        Estimate::~Estimate() {}
    }
}
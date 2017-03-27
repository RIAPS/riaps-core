//
// Created by istvan on 3/21/17.
//

#include <componentmodel/r_messagebase.h>
#include <stdexcept>

namespace riaps {
    MessageBase::MessageBase() {
        _isBuilder = true;
    }

    MessageBase::MessageBase(kj::ArrayPtr<const capnp::word>& array)  {
        _isBuilder = false;
        _bytesForReader = &array;
    }

    //void MessageBase::InitFields() {
    //    throw std::runtime_error("Invalid function call.");
    //}

    kj::ArrayPtr<const kj::ArrayPtr<const capnp::word>> MessageBase::GetBytes() {
        return _message.getSegmentsForOutput();
    }

    bool MessageBase::IsBuilder() {
        return _isBuilder;
    }

    MessageBase::~MessageBase() {

    }
}
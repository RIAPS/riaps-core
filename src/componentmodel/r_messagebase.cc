//
// Created by istvan on 3/21/17.
//

#include <componentmodel/r_messagebase.h>
#include <stdexcept>

namespace riaps {
    MessageBase::MessageBase() {
        _isBuilder = true;
    }

    //void MessageBase::ReadFromBytes(kj::ArrayPtr<const capnp::word> &array) {
    //    _isBuilder = false;
    //    _bytesForReader = &array;
    //}


    //void MessageBase::InitFields() {
    //    throw std::runtime_error("Invalid function call.");
    //}

    kj::Array<capnp::word> MessageBase::GetBytes() {
        return capnp::messageToFlatArray(_message);;
    }

    void MessageBase::SetAsReader() {
        _isBuilder = false;
    }

    bool MessageBase::IsBuilder() const {
        return _isBuilder;
    }

    MessageBase::~MessageBase() {

    }
}
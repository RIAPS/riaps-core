//
// Created by istvan on 3/21/17.
//

#include <componentmodel/r_messagebase.h>

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

    zmsg_t* MessageBase::AsZmqMessage() {
        auto serializedMessage = capnp::messageToFlatArray(_message);

        zmsg_t* msg = zmsg_new();
        auto size = serializedMessage.asBytes().size();
        auto bytes = serializedMessage.asBytes().begin();
        zmsg_pushmem(msg, bytes, size);
        return msg;
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
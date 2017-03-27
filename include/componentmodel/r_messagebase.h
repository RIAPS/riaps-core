//
// Created by istvan on 3/21/17.
//

#ifndef RIAPS_CORE_R_MESSAGEBASE_H
#define RIAPS_CORE_R_MESSAGEBASE_H

#include <capnp/message.h>
#include <capnp/serialize.h>

namespace riaps {

    class MessageBase {
    public:
        MessageBase();
        MessageBase(kj::ArrayPtr<const capnp::word>& array);

        //virtual void InitFields();

        virtual kj::ArrayPtr<const kj::ArrayPtr<const capnp::word>> GetBytes();

        ~MessageBase();

    protected:
        bool IsBuilder();

        ::capnp::MallocMessageBuilder _message;

        kj::ArrayPtr<const capnp::word>* _bytesForReader;

    private:
        bool _isBuilder;
    };
};

#endif //RIAPS_CORE_R_MESSAGEBASE_H

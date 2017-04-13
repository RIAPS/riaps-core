//
// Created by istvan on 3/21/17.
//

#ifndef RIAPS_CORE_R_MESSAGEBASE_H
#define RIAPS_CORE_R_MESSAGEBASE_H

#include <czmq.h>
#include <capnp/message.h>
#include <capnp/serialize.h>

namespace riaps {

    class MessageBase {
    public:
        MessageBase();

        //void ReadFromBytes(kj::ArrayPtr<const capnp::word>& array);
        virtual void InitReader(capnp::FlatArrayMessageReader* flatArrayMessageReader) = 0;

        //virtual void InitFields();

        //virtual kj::Array<capnp::word> GetBytes();

        zmsg_t* AsZmqMessage();

        ~MessageBase();

    protected:
        bool IsBuilder() const;
        void SetAsReader();

        ::capnp::MallocMessageBuilder _message;

       // kj::ArrayPtr<const capnp::word>* _bytesForReader;

    private:
        bool _isBuilder;
    };
};

#endif //RIAPS_CORE_R_MESSAGEBASE_H

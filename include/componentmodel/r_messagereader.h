//
// Created by istvan on 11/29/18.
//

#ifndef RIAPS_CORE_R_MESSAGEREADER_H
#define RIAPS_CORE_R_MESSAGEREADER_H

#include "ports/r_porterror.h"
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <czmq.h>
#include <memory>
#include <spdlog_setup/conf.h>

/**
 * Holds a ZMQ message. Provides kj::ArrayPtr structure for the content.
 */
class MessageReaderArray {
public:
    /**
     * Instantiates the object from ZMQ frame.
     * @param msg_frame ZMQ frame, whoch holds the message.
     */
    MessageReaderArray(zframe_t* msg_frame);
    MessageReaderArray( const MessageReaderArray& )            = delete; // non construction-copyable
    MessageReaderArray& operator=( const MessageReaderArray& ) = delete; // non copyable

    /**
     * Pointer to the content as kj::ArrayPtr (used by capnp).
     * @return
     */
    kj::ArrayPtr<const capnp::word> capnp_data() const;

    virtual ~MessageReaderArray();

private:
    zframe_t* frame_;
    size_t    frame_size_;
    byte*     data_;
    kj::ArrayPtr<const capnp::word> capnp_data_;
};

inline kj::ArrayPtr<const capnp::word> MessageReaderArray::capnp_data() const {
    return capnp_data_;
}

/**
 * Converts a MessageReaderArray into a typed capnp message.
 * @tparam T
 */
template <class T>
class MessageReader {
public:
    MessageReader() = delete;
    MessageReader(std::unique_ptr<MessageReaderArray>& reader) {
        reader_       = std::make_shared<capnp::FlatArrayMessageReader>(reader->capnp_data());
        reader_array_ = std::shared_ptr<MessageReaderArray>(reader.release());
        spec_reader_  = reader_->getRoot<T>();
    }

    /**
     * Function call on the wrapped capnp object.
     */
    typename T::Reader* operator->() {
        return &spec_reader_;
    }

    typename T::Reader& spec_reader(){
        return spec_reader_;
    }

    ~MessageReader() = default;
private:
    std::shared_ptr<MessageReaderArray>            reader_array_;
    std::shared_ptr<capnp::FlatArrayMessageReader> reader_;
    typename T::Reader            spec_reader_;
};

#endif //RIAPS_CORE_R_MESSAGEREADER_H

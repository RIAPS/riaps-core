//
// Created by istvan on 12/1/17.
//

#ifndef RIAPS_CORE_R_RIAPSMESSAGE_H
#define RIAPS_CORE_R_RIAPSMESSAGE_H

#include <utils/r_utils.h>
#include <czmq.h>
#include <capnp/serialize.h>
#include <capnp/message.h>

namespace riaps {

    //TODO: <R> is not needed. Refactor it.
    /**
     * Wraps a riaps message. ZMQ buffer, capnp buffer. Destroys the buffers on delete.
     * @tparam R Type of the capnp message.
     * @tparam T
     */
    template<class R, class T>
    class RiapsMessage{
    public:

        /**
         * Creates a RIAPS message from an incoming capnp message.
         * @param reader
         */
        RiapsMessage(capnp::FlatArrayMessageReader** reader){
            _capnpReader = *reader;
            _message = _capnpReader->getRoot<T>();
            _zframe = nullptr;
        }

        /**
         * Creates a RiapsMessage from ZMQ frame. Converts the ZMQ frame into type-specific capnp message.
         * @param zframe ZMQ frame.
         */
        RiapsMessage(zframe_t** zframe){
            _zframe      = *zframe;
            (*_zframe)>>_capnpReader;
            _message = _capnpReader->getRoot<T>();
        }


        /**
         * @return The type-specific capnp message.
         */
        R& GetMessage(){
            return *_message;
        }

        virtual ~RiapsMessage(){
            if (_capnpReader!= nullptr)
                delete _capnpReader;
            if (_zframe!= nullptr)
                zframe_destroy(&_zframe);
        }

    private:

        /**
         * Generic capnp buffer. It contains the capnp message.
         */
        capnp::FlatArrayMessageReader* _capnpReader;

        /**
         * Type-specific capnp message.
         */
        R                              _message;

        /**
         * ZMQ frame. This is what arrives on the port.
         */
        zframe_t*                      _zframe;
    };
}

#endif //RIAPS_CORE_R_RIAPSMESSAGE_H

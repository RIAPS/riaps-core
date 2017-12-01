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

    template<class R, class T>
    class RiapsMessage{
    public:

        RiapsMessage(capnp::FlatArrayMessageReader** reader){
            _capnpReader = *reader;
            _message = _capnpReader->getRoot<T>();
            _zframe = nullptr;
        }

        RiapsMessage(zframe_t** zframe){
            _zframe      = *zframe;
            (*_zframe)>>_capnpReader;
            _message = _capnpReader->getRoot<T>();
        }


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
        capnp::FlatArrayMessageReader* _capnpReader;
        R                              _message;
        zframe_t*                      _zframe;
    };
}

#endif //RIAPS_CORE_R_RIAPSMESSAGE_H

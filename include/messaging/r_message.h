//
// Created by istvan on 3/14/17.
//

#ifndef RIAPS_CORE_R_MESSAGE_H
#define RIAPS_CORE_R_MESSAGE_H

#include <czmq.h>
#include <memory>

template <class T>
class IRiapsMessage{
public:
    IRiapsMessage();
    virtual zmsg_t* RMessageToZmq(T message) = 0;
    virtual byte*   RMessageToByte(T message) = 0;
    virtual T ZmqToRMessage(zmsg_t* message) = 0;
    virtual T ByteToRMessage(byte* message) = 0;
    virtual ~IRiapsMessage();
};

#endif //RIAPS_CORE_R_MESSAGE_H

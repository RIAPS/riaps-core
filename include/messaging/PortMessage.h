//
// Created by istvan on 2/22/17.
//

#ifndef RIAPS_CORE_PORTMESSAGE_H
#define RIAPS_CORE_PORTMESSAGE_H

#include <map>

class PortMessage{
public:


protected:
    std::map _values;

public:
    PortMessage();
    ~PortMessage;
};

#endif //RIAPS_CORE_PORTMESSAGE_H

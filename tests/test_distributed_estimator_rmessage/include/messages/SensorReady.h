//
// Created by istvan on 3/14/17.
//

#ifndef RIAPS_CORE_SENSORREADY_H
#define RIAPS_CORE_SENSORREADY_H

#include <vector>
#include <messaging/r_message.h>

class SensorReady : public IRiapsMessage<std::string>
{
    SensorReady();

    zmsg_t* SerializeMessage(std::string message);

    std::string DeserializeMessage(zmsg_t* message);


    ~SensorReady();
};

#endif //RIAPS_CORE_SENSORREADY_H

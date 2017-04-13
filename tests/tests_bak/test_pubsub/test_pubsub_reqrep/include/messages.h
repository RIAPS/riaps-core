//
// Created by parallels on 9/15/16.
//

#ifndef RIAPS_FW_MESSAGES_H_H
#define RIAPS_FW_MESSAGES_H_H

#define MSG_ACT_PING "$PING$"
#define MSG_GPS      "$GPS$"
#define MSG_GPS_REQ  "$GPS_REQ$"

#include <czmq.h>

#include <string>
#include <vector>

zmsg_t* create_message(std::string messagetype, std::vector<std::string>& params);

void unpack_message(zmsg_t* msg, std::string& messagetype, std::vector<std::string>& params);

#endif //RIAPS_FW_MESSAGES_H_H

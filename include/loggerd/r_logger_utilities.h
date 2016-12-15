//
// Created by istvan on 11/13/16.
//

#ifndef RIAPS_FW_R_LOGGER_UTILITIES_H
#define RIAPS_FW_R_LOGGER_UTILITIES_H

#include <czmq.h>
#include <string>
#include "messaging/logger.capnp.h"
#include "r_logger_common.h"
#include <capnp/message.h>
#include <capnp/serialize.h>

void sendLogMessage(zsock_t* log_socket, std::string message);
zsock_t* initLogger();

#endif //RIAPS_FW_R_LOGGER_UTILITIES_H

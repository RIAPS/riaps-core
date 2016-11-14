#ifndef R_LOGGERD
#define R_LOGGERD

#include <czmq.h>
#include <iostream>
#include <string>
#include "utils/r_capnp_helper.h"
#include "messaging/logger.capnp.h"
#include "r_logger_common.h"




void sendLogRequest(zsock_t* log_socket, LogMessageType messageType);
zsock_t* initLogger();

#endif // R_LOGGERD
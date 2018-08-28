#ifndef RIAPS_CORE_R_CONST_H
#define RIAPS_CORE_R_CONST_H

#include <string>

constexpr auto ENV_RIAPSAPPS = "RIAPSAPPS";

//#define DISCOVERY_SERVICE_IPC "ipc:///tmp/riaps-disco"
constexpr auto DISCOVERY_ENDPOINT = "tcp://127.0.0.1:9700";


// Name of endpoint for actor-devm communication
//#define DEVMANAGER_SERVICE_ADDR "ipc:///tmp/riaps-devm"
constexpr auto DEVMANAGER_ENDPOINT = "tcp://127.0.0.1:9790";

//Timeout for actor-devm communication (-1: wait forever)
#define DEVM_ENDPOINTRECVTIMEOUT 1000
#define DEVM_ENDPOINTSENDTIMEOUT 3000

constexpr auto DEPLO_ENDPOINT = "tcp://127.0.0.1:9780";

/**
 * Actor argument index
 */
constexpr auto ARG_IDX_APP   = 1;
constexpr auto ARG_IDX_MODEL = 2;
constexpr auto ARG_IDX_ACTOR = 3;
constexpr auto ARG_IDX_DEVICE = ARG_IDX_ACTOR;

#endif //RIAPS_CORE_R_CONST_H

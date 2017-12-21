#ifndef RIAPS_CORE_R_CONST_H
#define RIAPS_CORE_R_CONST_H

#define ENV_RIAPSAPPS "RIAPSAPPS"

#define DISCOVERY_SERVICE_IPC "ipc:///tmp/riaps-disco"

// Name of endpoint for actor-devm communication
#define DEVMANAGER_SERVICE_IPC "ipc:///tmp/riaps-devm"

//Timeout for actor-devm communication (-1: wait forever)
#define DEVM_ENDPOINTRECVTIMEOUT 1000
#define DEVM_ENDPOINTSENDTIMEOUT 3000

/**
 * Actor argument index
 */
#define ARG_IDX_APP   1
#define ARG_IDX_MODEL 2
#define ARG_IDX_ACTOR 3
#define ARG_IDX_DEVICE ARG_IDX_ACTOR

#endif //RIAPS_CORE_R_CONST_H

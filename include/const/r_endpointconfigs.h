//
// Created by istvan on 5/11/17.
//

#ifndef RIAPS_CORE_R_ENDPOINTCONFIGS_H
#define RIAPS_CORE_R_ENDPOINTCONFIGS_H


#define DISCOVERY_SERVICE_IPC "ipc:///tmp/riaps-disco"

// Name of endpoint for actor-devm communication
#define DEVMANAGER_SERVICE_IPC "ipc:///tmp/riaps-devm"

//Timeout for actor-devm communication (-1: wait forever)
#define DEVM_ENDPOINTRECVTIMEOUT 1000
#define DEVM_ENDPOINTSENDTIMEOUT 3000


#endif //RIAPS_CORE_R_ENDPOINTCONFIGS_H

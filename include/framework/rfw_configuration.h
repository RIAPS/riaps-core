//
// Created by istvan on 2/27/17.
//

#ifndef RIAPS_CORE_RFW_CONFIGURATION_H
#define RIAPS_CORE_RFW_CONFIGURATION_H

#define DISCOVERY_SERVICE_IPC "ipc:///tmp/riaps-disco"

#include <string>

namespace riaps {
    namespace framework {
        class Configuration{
        public:
            static std::string GetDiscoveryServiceIpc();
        };
    }
}


#endif

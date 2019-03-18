//
// Created by istvan on 2/27/17.
//

#ifndef RIAPS_CORE_RFW_CONFIGURATION_H
#define RIAPS_CORE_RFW_CONFIGURATION_H

#include <INIReader.h>
#include <const/r_const.h>
#include <string>

namespace riaps {
    namespace framework {
        class Configuration{
        public:
            static const std::string GetDiscoveryEndpoint();
            static const std::string GetDeviceManagerEndpoint();
            static const std::string GetDeploEndpoint();
        };
    }
}


#endif

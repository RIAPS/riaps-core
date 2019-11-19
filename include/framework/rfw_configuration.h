//
// Created by istvan on 2/27/17.
//

#ifndef RIAPS_CORE_RFW_CONFIGURATION_H
#define RIAPS_CORE_RFW_CONFIGURATION_H

#include <INIReader.h>
#include <const/r_const.h>
#include <string>

namespace riaps {

    /**
     * Common functions.
     */
    namespace framework {
        class Configuration{
        public:

            /**
             * @return IP address : port of the discovery service.
             */
            static const std::string GetDiscoveryEndpoint();

            /**
             * @return IP address : port of the device manager.
             */
            static const std::string GetDeviceManagerEndpoint();

            /**
             * @return IP address : port of the deplo service.
             */
            static const std::string GetDeploEndpoint();
        };
    }
}


#endif

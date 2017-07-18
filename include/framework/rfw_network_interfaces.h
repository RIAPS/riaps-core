//
// Created by istvan on 2/27/17.
//

#ifndef RIAPS_CORE_RFW_NETWORK_INTERFACES_H
#define RIAPS_CORE_RFW_NETWORK_INTERFACES_H

#include <string>

namespace riaps {
    namespace framework{
        class Network{
            public:

            // Returns the MAC address of the given network interface. E.g.: eth0
            // If not specified, the ifacename will be the first global network interface
            static std::string
            GetMacAddress(const std::string& ifacename = "");


            // Returns the MAC address without ':' of the given network interface. E.g.: eth0
            // If not specified, the ifacename will be the first global network interface
            static std::string
            GetMacAddressStripped(const std::string& ifacename = "");

            // Returns the IP address of the given network interface. E.g.: eth0
            // If not specified, the ifacename will be first global network interface
            static std::string
            GetIPAddress(const std::string& ifacename = "");

            // Returns the IP address and name of the first global network interface
            static void
            GetFirstGlobalIface(std::string& ifacename, std::string& ipaddress);

            // Returns the nic_name parameter from the riaps.conf
            static std::string
            GetConfiguredIface();


            // Checks whether the given network interface exists
            static bool CheckIfaceName(const std::string& ifacename);

        };
    }
}

#endif //RIAPS_CORE_RFW_NETWORK_INTERFACES_H

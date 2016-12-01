//
// Created by istvan on 11/28/16.
//

#ifndef RIAPS_FW_R_NETWORK_INTERFACES_H
#define RIAPS_FW_R_NETWORK_INTERFACES_H

#include <string>

// Returns the MAC address of the given network interface. E.g.: eth0
// If not specified, the ifacename will be the first global network interface
extern std::string
GetMacAddress(const std::string& ifacename = "");


// Returns the MAC address without ':' of the given network interface. E.g.: eth0
// If not specified, the ifacename will be the first global network interface
extern std::string
GetMacAddressStripped(const std::string& ifacename = "");

// Returns the IP address of the given network interface. E.g.: eth0
// If not specified, the ifacename will be first global network interface
extern std::string
GetIPAddress(const std::string& ifacename = "");

// Returns the IP address and name of the first global network interface
extern void
GetFirstGlobalIface(std::string& ifacename, std::string& ipaddress);


#endif //RIAPS_FW_R_NETWORK_INTERFACES_H

//
// Created by istvan on 11/28/16.
//

#ifndef RIAPS_FW_R_NETWORK_INTERFACES_H
#define RIAPS_FW_R_NETWORK_INTERFACES_H

#include <string>

#define RIAPS_DEFAULT_IFACE std::string("enp0s5")

extern std::string
GetMacAddress(const std::string& ifacename);

extern std::string
GetMacAddressStripped(const std::string& ifacename);

extern std::string
GetIPAddress(const std::string& ifacename);

#endif //RIAPS_FW_R_NETWORK_INTERFACES_H

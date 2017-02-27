//
// Created by istvan on 11/28/16.
//

#include "componentmodel/delete_r_network_interfaces.h"
#include <fstream>
#include <algorithm>
#include <czmq.h>

// TODO: with C api not from file.
std::string GetMacAddress(const std::string& ifacename){

    std::string tmp_ifacename = ifacename;
    if (ifacename == ""){
        std::string ipaddress;
        GetFirstGlobalIface(tmp_ifacename, ipaddress);
    }

    std::string iface_path = "/sys/class/net/";
    std::string address_path = iface_path + tmp_ifacename + "/address";

    std::ifstream infile(address_path);
    char* macBuffer = new char[256];
    infile.getline(macBuffer, 256);
    infile.close();
    std::string mac_str = std::string(macBuffer);
    delete [] macBuffer;

    return mac_str;
}

std::string GetMacAddressStripped(const std::string& ifacename){
    std::string mac_address = GetMacAddress(ifacename);
    mac_address.erase(std::remove(mac_address.begin(), mac_address.end(), ':'), mac_address.end());

    return mac_address;
}

std::string GetIPAddress(const std::string& ifacename){

    if (ifacename == ""){
        std::string ipaddress;
        std::string tmp_ifacename;
        GetFirstGlobalIface(tmp_ifacename, ipaddress);

        return ipaddress;
    }

    ziflist_t *iflist = ziflist_new ();
    assert (iflist);

    const char *name = ziflist_first (iflist);
    std::string result = "";

    while (name && result == "") {
        std::string namestr(name);
        if (namestr == ifacename){
            result = ziflist_address (iflist);
        }
        name = ziflist_next (iflist);
    }

    ziflist_destroy(&iflist);

    return result;
}

// TODO: delete [] const char* in this case?
void GetFirstGlobalIface(std::string& ifacename, std::string& ipaddress){
    ziflist_t *iflist = ziflist_new();
    assert (iflist);

    const char *name = ziflist_first(iflist);
    bool result = false;

    while (name && !result) {
        std::string address(ziflist_address(iflist));
        if (address != "127.0.0.1") {
            ifacename = name;
            ipaddress = address;
            result = true;
        }
        name = ziflist_next(iflist);
    }

    ziflist_destroy(&iflist);

    if (!result){
        throw std::runtime_error("Cannot get details of the first global network interface.");
    }

}
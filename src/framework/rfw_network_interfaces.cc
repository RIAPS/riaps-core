//
// Created by istvan on 2/27/17.
//
#include <framework/rfw_network_interfaces.h>

#include <czmq.h>

#include <string>
#include <fstream>
#include <algorithm>
#include <regex>

namespace riaps{
    namespace framework{
        std::string Network::GetMacAddress(const std::string &ifacename) {
            std::string tmp_ifacename = ifacename;
            if (ifacename == ""){
                std::string ipaddress;

                // Get the netinterface to be used from the riaps.conf file
                std::string configuredIface = GetConfiguredIface();

                // If configured interface, get the first global
                if (configuredIface == "" || !CheckIfaceName(configuredIface))
                    GetFirstGlobalIface(tmp_ifacename, ipaddress);
                else tmp_ifacename = configuredIface;
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

        bool Network::CheckIfaceName(const std::string& ifacename) {
            if (ifacename=="") return false;
            std::string iface_path = "/sys/class/net/";
            std::string address_path = iface_path + ifacename + "/address";

            std::ifstream infile(address_path);
            return infile.good();
        }

        std::string Network::GetConfiguredIface() {
            std::string configPath = "/usr/local/riaps/etc/riaps.conf";
            std::ifstream f(configPath);

            if (f.good()){
                std::regex nicNameRegex("^nic_name\\s*=\\s*([a-zA-Z0-9]+)\\s*$");
                for(std::string line; getline( f, line ); )
                {
                    std::smatch match;
                    if (std::regex_search(line, match, nicNameRegex)){
                        f.close();
                        return match[1];
                    }
                }
            }

            f.close();
            return "";
        }

        void Network::GetFirstGlobalIface(std::string &ifacename, std::string &ipaddress) {
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

        std::string Network::GetIPAddress(const std::string &ifacename) {
            std::string configuredIface = GetConfiguredIface();
            std::string tmpIfacename = "";

            if (ifacename == ""){
                if (configuredIface == "" || !CheckIfaceName(configuredIface)){
                    std::string ipaddress;
                    GetFirstGlobalIface(tmpIfacename, ipaddress);
                    return ipaddress;
                } else {
                    tmpIfacename = configuredIface;
                }
            } else {
                tmpIfacename = ifacename;
            }

            ziflist_t *iflist = ziflist_new ();
            assert (iflist);

            const char *name = ziflist_first (iflist);
            std::string result = "";

            while (name && result == "") {
                std::string namestr(name);
                if (namestr == tmpIfacename){
                    result = ziflist_address (iflist);
                }
                name = ziflist_next (iflist);
            }

            ziflist_destroy(&iflist);

            return result;
        }

        std::string Network::GetMacAddressStripped(const std::string &ifacename) {
            std::string mac_address = GetMacAddress(ifacename);
            mac_address.erase(std::remove(mac_address.begin(), mac_address.end(), ':'), mac_address.end());

            return mac_address;
        }
    }
}
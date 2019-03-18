//
// Created by istvan on 2/27/17.
//

#include <framework/rfw_configuration.h>
#include <framework/rfw_network_interfaces.h>

using namespace std;

namespace riaps{
    namespace framework{
        const string Configuration::GetDiscoveryEndpoint() {
            auto result = DISCOVERY_ENDPOINT;// + Network::GetMacAddressStripped();
            return result;
        }

        const string Configuration::GetDeviceManagerEndpoint(){
            auto result = DEVMANAGER_ENDPOINT;// + Network::GetMacAddressStripped();
            return result;
        }

        const string Configuration::GetDeploEndpoint() {
            auto result = DEPLO_ENDPOINT;// + Network::GetMacAddressStripped();
            return result;
        }
    }
}
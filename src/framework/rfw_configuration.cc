//
// Created by istvan on 2/27/17.
//

#include <framework/rfw_configuration.h>
#include <framework/rfw_network_interfaces.h>


namespace riaps{
    namespace framework{
        const std::string Configuration::GetDiscoveryEndpoint() {
            auto result = DISCOVERY_ENDPOINT;// + Network::GetMacAddressStripped();
            return result;
        }

        const std::string Configuration::GetDeviceManagerEndpoint(){
            auto result = DEVMANAGER_ENDPOINT;// + Network::GetMacAddressStripped();
            return result;
        }

        const std::string Configuration::GetDeploEndpoint() {
            auto result = DEPLO_ENDPOINT;// + Network::GetMacAddressStripped();
            return result;
        }
    }
}
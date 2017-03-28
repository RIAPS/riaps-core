//
// Created by istvan on 2/27/17.
//

#include <framework/rfw_configuration.h>
#include <framework/rfw_network_interfaces.h>

#include <boost/format.hpp>

namespace riaps{
    namespace framework{
        std::string Configuration::GetDiscoveryServiceIpc() {
            auto result = DISCOVERY_SERVICE_IPC + Network::GetMacAddressStripped();
            return result;
        }


    }
}
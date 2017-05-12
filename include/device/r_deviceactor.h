//
// Created by istvan on 5/8/17.
//

#ifndef RIAPS_CORE_R_DEVICE_H
#define RIAPS_CORE_R_DEVICE_H

#include <componentmodel/r_actor.h>
#include <json.h>
#include <set>
#include <string>
#include <vector>


namespace riaps {

    class DeviceActor {

    public:
        ~DeviceActor();

        static DeviceActor* CreateDeviceActor(nlohmann::json& configJson,
                                    const std::string& deviceName,
                                    std::map<std::string, std::string>& actualParams);

    private:
        DeviceActor(const std::string&     applicationName   ,
                    const std::string&     deviceActorName   ,
                    nlohmann::json&        jsonActorConfig   ,
                    nlohmann::json&        jsonDevicesConfig ,
                    std::map<std::string, std::string>& commandLineParams
        );

        std::string _applicationName;
        std::string _deviceActorName;

        std::vector<component_conf_j> _component_configurations;
    };
}

#endif //RIAPS_CORE_R_DEVICE_H

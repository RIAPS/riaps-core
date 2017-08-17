//
// Created by istvan on 5/5/17.
//

#include <device/r_deviceactor.h>
#include <const/r_jsonmodel.h>
#include <componentmodel/r_argumentparser.h>

namespace riaps{

    DeviceActor::DeviceActor(const std::string&     applicationname,
                             const std::string&     actorName,
                             const std::string&     deviceName,
                             const std::string&     jsonFile       ,
                             nlohmann::json         jsonActorconfig,
                             nlohmann::json&        configJson     ,
                             std::map<std::string, std::string>& commandLineParams)
            : Actor(applicationname,
                    actorName,
                    jsonFile,
                    jsonActorconfig,
                    configJson,
                    commandLineParams) {
        _startDevice = true;
        _deviceName  = deviceName;
    }

    DeviceActor* DeviceActor::CreateDeviceActor(nlohmann::json& configJson  ,
                                                const std::string& deviceName,
                                                const std::string& jsonFile ,
                                                std::map<std::string, std::string>& actualParams) {

        std::string applicationName    = configJson[J_NAME];
        nlohmann::json jsonActors      = configJson[J_ACTORS];
        std::string actorName          = "";

        // Find the actor
//        if (jsonActors.find(actorName)==jsonActors.end()){
//            std::cerr << "Didn't find actor in the model file: " << actorName << std::endl;
//            return NULL;
//        }

        for (auto it = jsonActors.begin(); it!=jsonActors.end(); it++){

            auto instances = (*it)[J_INSTANCES];
            // No device with deviceName in this actor
            if (instances.find(deviceName)==instances.end()) continue;

            actorName = it.key();
        }

        if (actorName == ""){
            std::cerr << "Didn't find actor of the device: " << deviceName << std::endl;
            return NULL;
        }

        auto jsonCurrentActor = jsonActors[actorName];

        return new ::riaps::DeviceActor(
                applicationName,
                actorName,
                deviceName,
                jsonFile,
                jsonCurrentActor,
                configJson,
                actualParams
        );



    }

    DeviceActor::~DeviceActor() {

    }
}
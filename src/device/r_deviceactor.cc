//
// Created by istvan on 5/5/17.
//

#include <device/r_deviceactor.h>
#include <const/r_jsonmodel.h>
#include <componentmodel/r_argumentparser.h>

namespace riaps{

    DeviceActor::DeviceActor(const std::string&     applicationname,
                             const std::string&     actorname      ,
                             const std::string&     jsonFile       ,
                             nlohmann::json         jsonActorconfig,
                             nlohmann::json&        configJson     ,
                             std::map<std::string, std::string>& commandLineParams)
            : Actor(applicationname,
                    actorname,
                    jsonFile,
                    jsonActorconfig,
                    configJson,
                    //jsonComponentsconfig,
                    //jsonDevicesconfig,
                    //jsonMessagesconfig,
                    commandLineParams) {
        _startDevice = true;
    }

    DeviceActor* DeviceActor::CreateDeviceActor(nlohmann::json& configJson  ,
                                                const std::string& actorName,
                                                const std::string& jsonFile ,
                                                std::map<std::string, std::string>& actualParams) {

        std::string applicationName    = configJson[J_NAME];
        nlohmann::json jsonActors      = configJson[J_ACTORS];


        // Find the actor
        if (jsonActors.find(actorName)==jsonActors.end()){
            std::cerr << "Didn't find actor in the model file: " << actorName << std::endl;
            return NULL;
        }

        auto jsonCurrentActor = jsonActors[actorName];

        return new ::riaps::DeviceActor(
                applicationName,
                actorName,
                jsonFile,
                jsonCurrentActor,
                configJson,
                actualParams
        );



    }

    DeviceActor::~DeviceActor() {

    }
}
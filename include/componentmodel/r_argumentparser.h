/*
 *
 * Author: Istvan Madari
 *
 */

#ifndef RIAPS_CORE_R_ARGUMENTPARSER_H
#define RIAPS_CORE_R_ARGUMENTPARSER_H

#include <componentmodel/r_parameter.h>
#include <const/r_jsonmodel.h>
#include "json.h"
#include <map>
#include <string>
#include <algorithm>

// TODO: Move into namespace (probably utils?)
class ArgumentParser{

public:
    ArgumentParser(std::map<std::string, std::string>& commandLineParams);

    /**
     *
     * @param name Component or device name
     * @return
     */
    virtual riaps::componentmodel::Parameters Parse(const std::string& name) = 0;

    virtual ~ArgumentParser();

protected:

    riaps::componentmodel::Parameters GetComponentFormals(nlohmann::json& jsonFormals);


    std::map<std::string, std::string>& _commandLineParams;

};

class ComponentArgumentParser : public ArgumentParser {
public:
    ComponentArgumentParser(std::map<std::string, std::string>& commandLineParams,
                            nlohmann::json& jsonActorConfig,
                            nlohmann::json& jsonComponentsConfig,
                            const std::string& actorName);

    riaps::componentmodel::Parameters Parse(const std::string& name);



    ~ComponentArgumentParser();
private:

//    riaps::componentmodel::Parameters GetComponentActuals(nlohmann::json&                    json_componentactuals,
//                                                          riaps::componentmodel::Parameters& actorParams);

    std::vector<riaps::componentmodel::ComponentActual> GetComponentActuals(nlohmann::json& json_componentactuals);

    nlohmann::json&                     _json_actorconfig;
    nlohmann::json&                     _json_componentsconfig;
    std::string                         _actorname;

};

class DeviceArgumentParser : public ArgumentParser {
public:
    DeviceArgumentParser(std::map<std::string, std::string>& commandLineParams, nlohmann::json& jsonDeviceConfig);
    riaps::componentmodel::Parameters Parse(const std::string& deviceName);
    ~DeviceArgumentParser();
private:
    //std::string deviceName;
    nlohmann::json& _jsonDeviceConfig;
};

#endif //RIAPS_CORE_R_ARGUMENTPARSER_H

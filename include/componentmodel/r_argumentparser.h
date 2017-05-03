//
// Created by istvan on 5/3/17.
//

#ifndef RIAPS_CORE_R_ARGUMENTPARSER_H
#define RIAPS_CORE_R_ARGUMENTPARSER_H

#include <componentmodel/r_parameter.h>
#include <const/r_jsonmodel.h>
#include "json.h"
#include <map>
#include <string>
#include <algorithm>

class ArgumentParser{

public:
    ArgumentParser(std::map<std::string, std::string>& commandLineParams,
                   nlohmann::json&                     json_actorconfig,
                   nlohmann::json&                     json_componentsconfig);

    // Process and check actor parameters
    riaps::componentmodel::Parameters Parse(const std::string& componentName);



    ~ArgumentParser();

private:
    riaps::componentmodel::Parameters GetComponentFormals(nlohmann::json&                    jsonFormals);
    riaps::componentmodel::Parameters GetComponentActuals(nlohmann::json&                    json_componentactuals,
                                                          riaps::componentmodel::Parameters& actorParams);


    nlohmann::json&                     _json_actorconfig;
    nlohmann::json&                     _json_componentsconfig;
    std::map<std::string, std::string>& _commandLineParams;
};

#endif //RIAPS_CORE_R_ARGUMENTPARSER_H

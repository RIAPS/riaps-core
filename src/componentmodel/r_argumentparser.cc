//
// Created by istvan on 5/3/17.
//

#include <componentmodel/r_argumentparser.h>
#include <componentmodel/r_configuration.h>
#include <spdlog/spdlog.h>

namespace spd = spdlog;

ArgumentParser::ArgumentParser(std::map<std::string, std::string> &commandLineParams)
                        :
                          _commandLineParams(commandLineParams)

                          {

}

riaps::componentmodel::Parameters ArgumentParser::GetComponentFormals(nlohmann::json &jsonFormals) {
    Parameters results;

    for (auto it_formal = jsonFormals.begin();
         it_formal != jsonFormals.end();
         it_formal++){
        std::string formalName = (it_formal.value())[J_FORMAL_NAME];
        bool hasDefault = it_formal.value()[J_FORMAL_DEF] != NULL;

        std::string formalDefault = "";

        // Must check the type
        if (hasDefault){
            auto j_formalValue = (it_formal.value())[J_FORMAL_DEF];
            if (j_formalValue.is_string()){
                formalDefault = j_formalValue;
            } else if (j_formalValue.is_boolean()){
                formalDefault = std::to_string((bool)j_formalValue);
            } else if (j_formalValue.is_number()){
                formalDefault = std::to_string((double)j_formalValue);
            }
        }

        // If default value is specified, then the parameter is not mandatory.
        results.addParam(formalName, "", hasDefault, formalDefault);
    }

    return results;
}



ArgumentParser::~ArgumentParser() {

}

ComponentArgumentParser::ComponentArgumentParser(std::map<std::string, std::string> &commandLineParams,
                                                 nlohmann::json& jsonActorConfig,
                                                 nlohmann::json& jsonComponentsConfig,
                                                 const std::string& actorName)
        : ArgumentParser(commandLineParams),
          _json_actorconfig(jsonActorConfig),
          _json_componentsconfig(jsonComponentsConfig),
          _actorname(actorName){

}

ComponentArgumentParser::~ComponentArgumentParser() {

}

riaps::componentmodel::Parameters ComponentArgumentParser::Parse(const std::string& name) {


    auto json_instances     = _json_actorconfig[J_INSTANCES];
    auto json_internals     = _json_actorconfig[J_INTERNALS];
    auto json_locals        = _json_actorconfig[J_LOCALS];
    auto json_actor_formals = _json_actorconfig[J_FORMALS];


    riaps::componentmodel::Parameters actorParams;


    for (auto it_actor_formal = json_actor_formals.begin();
         it_actor_formal != json_actor_formals.end();
         it_actor_formal++){


        std::string formalName = (it_actor_formal.value())[J_FORMAL_NAME];

        // The parameter is optional iff the "default" key is defined in the config
        bool isOptional = it_actor_formal.value()[J_FORMAL_DEF] != NULL;

        // Set the default to empty string if no default value is defined in the config
        // Othervise set the configured value (as string, we are dealing with types later)
        std::string formalDefault = isOptional ? (it_actor_formal.value())[J_FORMAL_DEF] : "";

        // Optional
        if (isOptional){
            // Optional parameter is not passed to the actor
            // Set it to the default value
            if (_commandLineParams.find(formalName) == _commandLineParams.end()){
                actorParams.addParam(formalName, "", isOptional, formalDefault);
            }

            // Optional parameter is passed, set the parameter to the passed value
            else {
                actorParams.addParam(formalName, _commandLineParams[formalName], isOptional, formalDefault);
            }
        }

        // Mandatory
        else {
            if (!isOptional && _commandLineParams.find(formalName) == _commandLineParams.end()){
                throw std::invalid_argument("Mandatory parameter is missing from the actor: " +
                                            _actorname +
                                            "(" + formalName + ")"
                );
            }
            else {
                actorParams.addParam(formalName, _commandLineParams[formalName], isOptional, formalDefault);
            }
        }
    }

    // Get the components
    //std::cout << json_instances <<std::endl;
    auto json_compinst = json_instances[name];
    //std::cout << json_compinst <<std::endl <<std::flush;
    std::string componentType   = json_compinst[J_TYPE];
    auto json_componentactuals = json_compinst[J_ACTUALS];
    auto componentActuals = GetComponentActuals(json_componentactuals);

    riaps::componentmodel::Parameters resolvedComponentParams;

    // Resolve actual params
    for (auto it_actual = componentActuals.begin();
              it_actual!= componentActuals.end();
              it_actual++){

        // Actual value refers to a previously passed parameter -> resolve
        if (it_actual->HasParamReferred()) {
            auto currentActorParam = actorParams.getParam(it_actual->GetReferredParamName());

            // Something is wrong, missing argument
            if (currentActorParam == NULL){
                throw std::invalid_argument("Missing argument: "      +
                                            it_actual->GetReferredParamName() +
                                            " for component "         +
                                            name);
            }

            // Found it, create the resolved param
            Parameter p(it_actual->GetParamName(), currentActorParam->isOptional());
            p.setValue(currentActorParam->getValueAsString());
            resolvedComponentParams.addParam(p);
        }

        // Just value, leave it as is
        else {
            Parameter p(it_actual->GetParamName(), false);
            p.setValue(it_actual->GetParamValue());
            resolvedComponentParams.addParam(p);
        }


    }

    // Get the component formals
    auto json_componentconfig = _json_componentsconfig[componentType];
    auto json_componentformals = json_componentconfig[J_FORMALS];
    auto componentFormals = GetComponentFormals(json_componentformals);
    auto componentFormalNames = componentFormals.getParameterNames();

    // Check if every parameter was passed. If not, check if they are optional.
    // If not passed and not optional -> error.

    for (auto it_formal = componentFormalNames.begin();
              it_formal != componentFormalNames.end();
              it_formal++){
        auto currentFormal = componentFormals.getParam(*it_formal);

        // Yep, the parameter already exist
        if (resolvedComponentParams.getParam(currentFormal->getName())!=NULL){
            continue;
        }
        // The parameter hasn't passed. Check if it is optional
        else if (currentFormal->isOptional()){
            // Yes it is optional, set the default value
            Parameter p(currentFormal->getName(), currentFormal->isOptional(), currentFormal->getDefaultValue());
            p.setValue(currentFormal->getDefaultValue());
            resolvedComponentParams.addParam(p);
        }

        // Not optional and not passed. Error!
        else {
            throw std::invalid_argument("Parameter: " + currentFormal->getName() + " is mandatory, but not passed.");
        }
    }

    return resolvedComponentParams;
}




std::vector<riaps::componentmodel::ComponentActual> ComponentArgumentParser::GetComponentActuals(nlohmann::json& json_componentactuals) {
    std::vector<riaps::componentmodel::ComponentActual> results;

    // Check the component actuals
    for (auto it_actual = json_componentactuals.begin();
         it_actual!= json_componentactuals.end();
         it_actual++) {

        std::string actualName = (it_actual.value())[J_ACTUAL_NAME];

        auto j_actualParam = (it_actual.value())[J_ACTUAL_PARAM];
        bool hasActualParam = j_actualParam != NULL;
        std::string actualParam = !hasActualParam ? "" : j_actualParam;

        auto j_actualValue = (it_actual.value())[J_ACTUAL_VALUE];
        bool hasActualValue = j_actualValue != NULL;
        std::string actualValue = "";

        // Must check the type
        if (hasActualValue) {
            if (j_actualValue.is_string()) {
                actualValue = j_actualValue;
            } else if (j_actualValue.is_boolean()) {
                actualValue = std::to_string((bool) j_actualValue);
            } else if (j_actualValue.is_number()) {
                actualValue = std::to_string((double) j_actualValue);
            }
            ComponentActual act(actualName, actualValue, false);
            results.push_back(act);
        } else if (hasActualParam){
            ComponentActual act(actualName, actualParam, true);
            results.push_back(act);
        }
    }

    return results;
}


DeviceArgumentParser::DeviceArgumentParser(std::map<std::string, std::string> &commandLineParams,
                                           nlohmann::json& jsonDeviceConfig)
    : ArgumentParser(commandLineParams), _jsonDeviceConfig(jsonDeviceConfig){
}

riaps::componentmodel::Parameters DeviceArgumentParser::Parse(const std::string &deviceName) {
    auto jsonDeviceFormals = _jsonDeviceConfig[J_FORMALS];
    auto deviceFormals = GetComponentFormals(jsonDeviceFormals);
    auto deviceFormalNames = deviceFormals.getParameterNames();
    auto logger = spd::get(deviceName);

    riaps::componentmodel::Parameters resolvedComponentParams;


    for (auto it_formal = deviceFormalNames.begin();
         it_formal != deviceFormalNames.end();
         it_formal++){
        auto currentFormal = deviceFormals.getParam(*it_formal);

        if (_commandLineParams.find(currentFormal->getName()) == _commandLineParams.end() && !currentFormal->isOptional()){
            logger->error("Parameter {} is mandatory but missing (for device: {}).", currentFormal->getName(), deviceName);
            continue;
        }

        Parameter p(currentFormal->getName(), currentFormal->isOptional(), currentFormal->getDefaultValue());
        p.setValue(currentFormal->getDefaultValue());
        resolvedComponentParams.addParam(p);
    }

    // Actuals are the command line params
    for (auto it = _commandLineParams.begin();
              it != _commandLineParams.end();
              it++){
        if (resolvedComponentParams.getParam(it->first) == nullptr){
            logger->error("Parameter {} is passed but no formal definition found (for device: {})", it->first, deviceName);
            continue;
        }

        resolvedComponentParams.setParamValue(it->first, it->second);
    }

    return resolvedComponentParams;
}

DeviceArgumentParser::~DeviceArgumentParser() {

}
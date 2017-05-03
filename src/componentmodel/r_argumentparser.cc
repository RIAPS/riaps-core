//
// Created by istvan on 5/3/17.
//

#include <componentmodel/r_argumentparser.h>
#include <componentmodel/r_configuration.h>

ArgumentParser::ArgumentParser(std::map<std::string, std::string> &commandLineParams,
                               nlohmann::json &json_actorconfig,
                               nlohmann::json &json_componentsconfig)
                        : _json_actorconfig(json_actorconfig),
                          _commandLineParams(commandLineParams),
                          _json_componentsconfig(json_componentsconfig){

}

riaps::componentmodel::Parameters ArgumentParser::Parse(const std::string& componentName) {


    auto json_instances = _json_actorconfig[J_INSTANCES];
    auto json_internals = _json_actorconfig[J_INTERNALS];
    auto json_locals    = _json_actorconfig[J_LOCALS];
    auto json_actor_formals   = _json_actorconfig[J_FORMALS];


    // TODO:
    std::string actorname = "";

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
                actorParams.AddParam(formalName, "", isOptional, formalDefault);
            }

            // Optional parameter is passed, set the parameter to the passed value
            else {
                actorParams.AddParam(formalName, _commandLineParams[formalName], isOptional, formalDefault);
            }
        }

        // Mandatory
        else {
            if (!isOptional && _commandLineParams.find(formalName) == _commandLineParams.end()){
                throw std::invalid_argument("Mandatory parameter is missing from the actor: " +
                                            actorname +
                                            "(" + formalName + ")"
                );
            }
        }
    }

    // Get the components
    auto currentcomponentconfig = json_instances[componentName];
    std::string componentType   = currentcomponentconfig[J_TYPE];

    // Get the details of the component
    auto json_componentconfig = _json_componentsconfig[componentType];
    auto json_componentformals = json_componentconfig[J_FORMALS];
    auto json_componentactuals = currentcomponentconfig[J_ACTUALS];

    //_component_conf_j new_component_config;

    // Get the formals
    //new_component_config.component_parameters = GetComponentFormals(json_componentformals);
    //auto componentParameters = &new_component_config.component_parameters;

    auto componentFormals = GetComponentFormals(json_componentformals);
    auto componentActuals = GetComponentActuals(json_componentactuals, actorParams);

    // Check 1 - All the necessary formals are provided
    auto formalParameterNames = componentFormals.GetParameterNames();
//    for (auto it_formalparamname = formalParameterNames.begin();
//              it_formalparamname!=formalParameterNames.end();
//              it_formalparamname++){
//
//        if (componentActuals.GetParam(*it_formalparamname)==NULL){
//            throw std::invalid_argument("Parameter "
//                                        + *it_formalparamname
//                                        + " is missing for Component "
//                                        + componentName);
//        }
//    }

    // Go through the actuals, check if the paramater is passed or just assigned

    auto actualParameterNames  = componentActuals.GetParameterNames();
    auto actualActorParamNames = actorParams.GetParameterNames();

    riaps::componentmodel::Parameters resultComponentParameters;

    for (auto it_formalparamname = formalParameterNames.begin();
         it_formalparamname!=formalParameterNames.end();
         it_formalparamname++) {

        auto compFormalParamName = *it_formalparamname;

        // Is it in the actuals?
        if (componentActuals.GetParam(compFormalParamName)!=NULL){
            // Yes it is.
            // Is this parameter passed from the actor? Then try to resolve it

        }

        // Ok, not in the actuals, but it is optional?
        else if (componentFormals.GetParam(compFormalParamName)->IsOptional()) {
            // Great. It is optional, add the default value
            auto p = componentFormals.GetParam(compFormalParamName);
            resultComponentParameters.AddParam(*p);

        }
        else {
            throw std::invalid_argument("Formal parameter: "
                                      + compFormalParamName
                                      + " is not in the actuals.");
        }
    }

    for (auto it_actualname  = actualParameterNames.begin();
              it_actualname != actualParameterNames.end();
              it_actualname++){
        if ()
    }



//    {
//
//
//
//        // Check 1 : ActualName is defined in the ComponentFormals
//        if (componentParameters->GetParam(actualName) == NULL){
//            throw std::invalid_argument("Parameter "
//                                        + actualName
//                                        + " is missing from the Component "
//                                        + componentName
//                                        + " definition");
//        }
//
//        // Check 2 : Instead of values, passing a parameter. Check the paramater to be passed if exists or not.
//        if (hasActualParam){
//            if (actualActorParams.find(actualParam) == actualActorParams.end()){
//                throw std::invalid_argument("Parameter "
//                                            + actualParam
//                                            + " cannot be passed to component "
//                                            + componentName);
//            }
//
//            // Everything is fine, set the value
//            if (componentParameters->SetParamValue(actualName, actualActorParams[actualParam])==NULL){
//                throw std::invalid_argument("Parameter "
//                                            + actualParam
//                                            + " cannot be set in component "
//                                            + componentName);
//            }
//        } else if (componentParameters->SetParamValue(actualName, actualValue)==NULL){
//            throw std::invalid_argument("Value "
//                                        + actualValue
//                                        + " cannot be set in component "
//                                        + componentName);
//        }
//
//    }
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
        results.AddParam(formalName, "", hasDefault, formalDefault);
    }

    return results;
}

riaps::componentmodel::Parameters GetComponentActuals(nlohmann::json&                    json_componentactuals,
                                                      riaps::componentmodel::Parameters& actorParams) {
    Parameters results;

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
            results.AddParam(actualName, actualValue, false);
        } else if (hasActualParam){

            auto passedParam = actorParams.GetParam(actualName);
            // Should be passed from the actor. If not, error.
            if ( passedParam != NULL){
                results.AddParam(actualName, passedParam->GetValueAsString(), passedParam->IsOptional(), passedParam->GetDefaultValue());
            }
            else {
                throw std::invalid_argument(actualName + " is not present in actor arguments.");
            }
        }
    }

    return results;
}

ArgumentParser::~ArgumentParser() {

}
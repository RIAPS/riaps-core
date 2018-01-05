//
// Created by istvan on 5/9/17.
//

#ifndef RIAPS_CORE_R_COMMANDLINEPARSER_H
#define RIAPS_CORE_R_COMMANDLINEPARSER_H



#include <componentmodel/r_parameter.h>
#include <const/r_jsonmodel.h>
#include <const/r_const.h>
#include <utils/r_utils.h>
#include <json.h>

#include <map>
#include <string>
#include <algorithm>

// TODO: Move into namespace (probably utils?)
class CommandLineParser{

public:

    /// \param modelFile Path of the RIAPS model file
    /// \param actorName The actor to be parsed from the model file
    CommandLineParser(char** argv,
                      int    argc);


    int ParseActorParams(std::map<std::string, std::string>& actualParams,
                         std::string&    appName,
                         std::string&    actorName,
                         std::string&    modelFile,
                         nlohmann::json& jsonConfig);

    int ParseDeviceParams(std::map<std::string, std::string>& actualParams,
                          std::string&    appName,
                          std::string&    deviceName,
                          std::string&    modelFile,
                          nlohmann::json& jsonConfig);




    static bool CommandOptionExists(char** begin, char** end, const std::string& option);

    ~CommandLineParser();

private:

    /**
     * Loads the JSON file
     *
     * @param actualParams out Contains the commandline parameters in key-value format.
     * @param actorName    out The actor name, parsed from commandline arguments.
     * @param jsonConfig   out The model file content, parsed into JSON object.
     */
    int Parse(std::map<std::string, std::string>& actualParams,
              std::string&    appName,
              std::string&    actorName,
              std::string&    modelFile,
              nlohmann::json& jsonConfig);

    char** _argv;
    int    _argc;
};

#endif //RIAPS_CORE_R_COMMANDLINEPARSER_H

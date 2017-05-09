//
// Created by istvan on 5/9/17.
//

#ifndef RIAPS_CORE_R_COMMANDLINEPARSER_H
#define RIAPS_CORE_R_COMMANDLINEPARSER_H

#include <componentmodel/r_parameter.h>
#include <const/r_jsonmodel.h>
#include "json.h"
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

    /// \param actualParams out Contains the commandline parameters in key-value format.
    /// \param actorName    out The actor name, parsed from commandline arguments.
    /// \param jsonConfig   out The model file content, parsed into JSON object.
    int Parse(std::map<std::string, std::string>& actualParams,
              std::string&    actorName,
              nlohmann::json& jsonConfig);





    ~CommandLineParser();

private:
    char** _argv;
    int    _argc;
};

#endif //RIAPS_CORE_R_COMMANDLINEPARSER_H

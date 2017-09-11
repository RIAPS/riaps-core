//
// Created by istvan on 5/9/17.
//

#include <componentmodel/r_commandlineparser.h>
#include <regex>
#include <fstream>

CommandLineParser::CommandLineParser(char** argv, int argc)
    : _argv(argv), _argc(argc){

}

bool CommandLineParser::CommandOptionExists(char **begin, char **end, const std::string &option) {
    return std::find(begin, end, option) != end;
}

int CommandLineParser::ParseActorParams(std::map<std::string, std::string> &actualParams,
                                        std::string &appName,
                                        std::string &actorName,
                                        std::string &modelFile,
                                        nlohmann::json &jsonConfig) {
    return Parse(actualParams, appName, actorName, modelFile, jsonConfig);
}

int CommandLineParser::ParseDeviceParams(std::map<std::string, std::string> &actualParams,
                                         std::string &appName,
                                         std::string &deviceName,
                                         std::string &modelFile,
                                         nlohmann::json &jsonConfig) {
    return Parse(actualParams, appName, deviceName, modelFile, jsonConfig);
}

int CommandLineParser::Parse(std::map<std::string, std::string>& actualParams,
                             std::string& appName,
                             std::string& actorName,
                             std::string& modelFile,
                             nlohmann::json &jsonConfig) {

    // First param: <app>
    appName = std::string(_argv[ARG_IDX_APP]);

    // Second param: <model>
    modelFile = std::string(_argv[ARG_IDX_MODEL]);
    std::string modelFilePath;

    // RIAPSAPPS folder
    std::string appFolder = GetAppPath(appName);

    // Configured, try the configured folder
    if (appFolder!="") {
        modelFilePath = appFolder + "/" + modelFile;
    } else {
        modelFilePath = modelFile;
    }

    // Check the json file in the apps folder
    std::ifstream ifs(modelFilePath);

    if (!ifs.good()){
        std::cerr << "Cannot open modelfile: " << modelFile << std::endl;
        return -1;
    }

    actorName = std::string(_argv[ARG_IDX_ACTOR]);
    if (actorName.empty()){
        std::cerr << "Actorname cannot be empty string" << std::endl;
        return -1;
    }

    // Parse the model file
    try {
        jsonConfig = nlohmann::json::parse(ifs);
    }
    catch(std::invalid_argument& e){
        std::cerr << "Cannot parse: " << modelFile << std::endl;
        std::cerr << e.what() << std::endl;
        return -1;
    }


    for (int i = ARG_IDX_ACTOR+1; i<_argc; i++){
        std::string currentParam = std::string(_argv[i]);
        std::regex paramRegex("--(.+)=(.+)");

        std::smatch paramMatch;

        if (!std::regex_search(currentParam, paramMatch, paramRegex)) {
            std::cerr << "Wrong parameter: " << _argv[i] << std::endl;
            return -1;
        }

        if (paramMatch.size()<3){
            std::cerr << "Wrong parameter: " << _argv[i] << std::endl;
            return -1;
        }

        std::string paramName = paramMatch[1];
        std::string paramValue = paramMatch[2];

        actualParams[paramName] = paramValue;

    }
}

CommandLineParser::~CommandLineParser() {

}
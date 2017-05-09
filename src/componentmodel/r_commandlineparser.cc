//
// Created by istvan on 5/9/17.
//

#include <componentmodel/r_commandlineparser.h>
#include <regex>
#include <fstream>

CommandLineParser::CommandLineParser(char** argv, int argc)
    : _argv(argv), _argc(argc){

}

int CommandLineParser::Parse(std::map<std::string, std::string> &actualParams,
                              std::string &actorName,
                              nlohmann::json &jsonConfig) {

    // First param: <model>
    std::string modelfile = std::string(_argv[1]);
    std::ifstream ifs(modelfile);

    if (!ifs.good()){
        std::cerr << "Cannot open modelfile: " << modelfile << std::endl;
        return -1;
    }

    actorName = std::string(_argv[2]);
    if (actorName.empty()){
        std::cerr << "Actorname cannot be empty string" << std::endl;
        return -1;
    }

    // Parse the model file
    try {
        jsonConfig = nlohmann::json::parse(ifs);
    }
    catch(std::invalid_argument& e){
        std::cerr << "Cannot parse: " << modelfile << std::endl;
        std::cerr << e.what() << std::endl;
        return -1;
    }


    for (int i = 3; i<_argc; i++){
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
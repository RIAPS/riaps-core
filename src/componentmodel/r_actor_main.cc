//
// Created by istvan on 11/7/16.
//

#include "componentmodel/r_actor_main.h"

int main(int argc, char* argv[]) {

    if (cmdOptionExists(argv, argv+argc, "-h") || argc < 3){
        std::cout << "Usage: start_actor <model> <actor> [-s]" << std::endl;
        std::cout << std::endl;
        std::cout << std::setw(15) << "model" << std::endl << "\t\tModel file argument (.json)" << std::endl;
        std::cout << std::setw(15) << "actor" << std::endl << "\t\tActor name argument" << std::endl;
        std::cout << std::setw(15) << "-s, --suffix" << std::endl << "\t\tDiscovery endpoint suffix" << std::endl;
        return 0;
    } else if (cmdOptionExists(argv, argv+argc, "-s") || cmdOptionExists(argv, argv+argc, "--suffix")){
        //TODO: finish
    }
    else {
        // First param: <model>
        std::string modelfile = std::string(argv[1]);
        std::ifstream ifs(modelfile);

        if (!ifs.good()){
            std::cerr << "Cannot open modelfile: " << modelfile << std::endl;
            return -1;
        }

        std::string actorname = std::string(argv[2]);
        if (actorname.empty()){
            std::cerr << "Actorname cannot be empty string" << std::endl;
            return -1;
        }

        nlohmann::json config_json;

        // Parse the model file
        try {
            config_json = nlohmann::json::parse(ifs);
        }
        catch(std::invalid_argument& e){
            std::cerr << "Cannot parse: " << modelfile << std::endl;
            std::cerr << e.what() << std::endl;
            return -1;
        }

        // Get the model parameters
        std::string applicationName;

        try {
            applicationName = config_json["name"];

            auto json_actors     = config_json["actors"];
            auto json_components = config_json["components"];
            auto json_messages   = config_json["messages"];

            // Find the actor to be started
            if (json_actors.find(actorname)==json_actors.end()){
                std::cerr << "Didn't find actor in the model file: " << actorname << std::endl;
                return -1;
            }

            auto json_currentactor = json_actors[actorname];
        }
        catch(std::domain_error& e){
            std::cerr << "Configuration file error (probably missing property from the json file)" << std::endl;
            std::cerr << e.what() << std::endl;
        }
    }

    return 0;
}

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}
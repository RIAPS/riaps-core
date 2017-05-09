//
// Created by istvan on 11/7/16.
//

#include "componentmodel/r_actor_main.h"
#include <regex>

#include <sched.h>
#include <componentmodel/r_commandlineparser.h>

int main(int argc, char* argv[]) {

    // Logger experiment
    //zsock_t* logger_socket = initLogger();
    //sendLogMessage(logger_socket, "Actor started...");
    //sched_param x;
    //x.__sched_priority = 99;
    //auto ret = sched_setscheduler(0, SCHED_FIFO, &x);

    //assert(ret==0);

    if (cmdOptionExists(argv, argv+argc, "-h") || argc < 3){
        std::cout << "Usage: start_actor <model> <actor> [-h]" << std::endl;
        std::cout << std::endl;
        std::cout << std::setw(15) << "model" << std::endl << "\t\tModel file argument (.json)" << std::endl;
        std::cout << std::setw(15) << "actor" << std::endl << "\t\tActor name argument" << std::endl;
        return 0;
    }
    else {


        // get the rest of the params
        std::map<std::string, std::string> actualParams;
        std::string actorName;
        nlohmann::json configJson;

        CommandLineParser cmdLineParser(argv, argc);
        if (cmdLineParser.Parse(actualParams, actorName, configJson) == -1){
            std::cerr << "Couldn't parse commandline parameters" << std::endl;
            return -1;
        }

        try {
            riaps::Actor* aptr = riaps::Actor::CreateActor(configJson, actorName, actualParams);
            std::unique_ptr<riaps::Actor> actor = std::unique_ptr<riaps::Actor>(aptr);
            actor->Init();
            actor->start();
        }
        catch(std::domain_error& e){
            std::cerr << "Configuration file error (probably missing property from the json file)" << std::endl;
            std::cerr << e.what() << std::endl;
        }
        catch (std::invalid_argument& e){
            std::cerr << e.what() << std::endl;
        }
    }

    //zsock_destroy(&logger_socket);

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
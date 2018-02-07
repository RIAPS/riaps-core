

#include <componentmodel/r_commandlineparser.h>
#include "componentmodel/r_actor_main.h"
#include <regex>
#include <sched.h>

int main(int argc, char* argv[]) {
    //Note: Uncomment for RT-FIFO
    sched_param x;
    x.__sched_priority = 99;
    auto ret = sched_setscheduler(0, SCHED_FIFO, &x);

    assert(ret==0);

    if (CommandLineParser::CommandOptionExists(argv, argv+argc, "-h") || argc < 4){
        std::cout << "Usage: start_actor <model> <actor> [-h]" << std::endl;
        std::cout << std::endl;
        std::cout << std::setw(15) << "app" << std::endl << "\t\tApplication name" << std::endl;
        std::cout << std::setw(15) << "model" << std::endl << "\t\tModel file argument (.json)" << std::endl;
        std::cout << std::setw(15) << "actor" << std::endl << "\t\tActor name argument" << std::endl;
        return 0;
    }
    else {


        // get the rest of the params
        std::map<std::string, std::string> actualParams;
        std::string appName;
        std::string actorName;
        std::string modelName;
        nlohmann::json configJson;

        CommandLineParser cmdLineParser(argv, argc);
        if (cmdLineParser.ParseActorParams(actualParams, appName, actorName, modelName, configJson) == -1){
            std::cerr << "Couldn't parse commandline parameters" << std::endl;
            return -1;
        }

        std::shared_ptr<spd::logger> _logger = spd::stdout_color_st(actorName);
        
        try {

            riaps::Actor* aptr = riaps::Actor::CreateActor(configJson, actorName, modelName, actualParams);

            

            std::unique_ptr<riaps::Actor> actor = std::unique_ptr<riaps::Actor>(aptr);
            if (actor->Init())
                actor->start();
        }
        catch(std::domain_error& e){
            _logger->error("Configuration file error (probably missing property from the json file)");
            _logger->error(e.what());
        }
        catch (std::invalid_argument& e){
            _logger->error(e.what());
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
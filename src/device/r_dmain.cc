//
// Created by istvan on 5/8/17.
//

#include <componentmodel/r_commandlineparser.h>
#include <device/r_deviceactor.h>
#include <regex>
#include <sched.h>


int main(int argc, char* argv[]) {

    if (CommandLineParser::CommandOptionExists(argv, argv+argc, "-h") || argc < 4){
        std::cout << "Usage: start_device <model> <device> [-h]" << std::endl;
        std::cout << std::endl;
        std::cout << std::setw(15) << "app" << std::endl << "\t\tApplication name" << std::endl;
        std::cout << std::setw(15) << "model" << std::endl << "\t\tModel file argument (.json)" << std::endl;
        std::cout << std::setw(15) << "device" << std::endl << "\t\tThe device to be instantiated" << std::endl;
        return 0;
    }
    else {


        // get the rest of the params
        std::map<std::string, std::string> actualParams;
        std::string deviceName;
        std::string modelName;
        std::string appName;
        nlohmann::json configJson;

        CommandLineParser cmdLineParser(argv, argc);
        if (cmdLineParser.ParseDeviceParams(actualParams, appName, deviceName,modelName, configJson) == -1){
            std::cerr << "Couldn't parse commandline parameters" << std::endl;
            return -1;
        }

        try {
            riaps::DeviceActor* dptr =
                    riaps::DeviceActor::CreateDeviceActor(configJson  ,
                                                          deviceName  ,
                                                          modelName   ,
                                                          actualParams);
            std::unique_ptr<riaps::DeviceActor> device = std::unique_ptr<riaps::DeviceActor>(dptr);
            dptr->Init();
            dptr->start();
            delete dptr;
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
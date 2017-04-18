//
// Created by istvan on 11/11/16.
//

#include "Generator.h"
#include <capnp/serialize.h>
#include <capnp/message.h>

namespace timertest {
    namespace components {

        Generator::Generator(_component_conf_j &config, riaps::Actor &actor) : GeneratorBase(config, actor) {

        }

        void Generator::OnClock(riaps::ports::PortBase *port) {

            if (_counter < 500)
                _results[_counter++] = std::chrono::high_resolution_clock::now();
            else {
                std::fstream _logStream;
                _logStream.open("./results.log", std::fstream::out);
                for (int i =1; i<500; i++){
                    auto d = std::chrono::duration_cast<std::chrono::nanoseconds>(_results[i]-_results[i-1]).count();
                    _logStream << d << std::endl;
                }
                _logStream.close();
                kill(getpid(), SIGKILL);
            }
//            capnp::MallocMessageBuilder messageBuilder;
//            auto msgSensorReady = messageBuilder.initRoot<messages::SensorReady>();
//            msgSensorReady.setMsg("data_ready");
//
//            SendReady(messageBuilder, msgSensorReady);
        }



        Generator::~Generator() {

        }
    }
}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    auto result = new timertest::components::Generator(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
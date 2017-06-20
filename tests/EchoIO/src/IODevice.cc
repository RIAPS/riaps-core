//
// Created by istvan on 6/20/17.
//

#include <IODevice.h>

namespace echoio{
    namespace components {

        IODevice::IODevice(_component_conf_j &config, riaps::Actor &actor)
            : IODeviceBase(config, actor){

        }

        void IODevice::OnClock(riaps::ports::PortBase *port){
            std::cout << "IODevice::OnClock()" << std::endl;
        }

        void IODevice::OnEcho(const messages::EchoRep::Reader &message, riaps::ports::PortBase *port) {

        }

        void IODevice::OnOneShotTimer(const std::string &timerid) {

        }

        IODevice::~IODevice() {}
    }
}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    auto result = new echoio::components::IODevice(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
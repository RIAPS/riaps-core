//
// Created by istvan on 5/16/17.
//
#include <GpioDeviceComponent.h>

namespace gpiotoggleexample{
    namespace components{
        GpioDeviceComponent::GpioDeviceComponent(_component_conf_j &config, riaps::Actor &actor)
            : GpioDeviceComponentBase(config, actor){

        }

        void GpioDeviceComponent::OnReadGpio(const messages::ReadRequest::Reader &message,
                                             riaps::ports::PortBase *port) {

        }

        void GpioDeviceComponent::OnWriteGpio(const messages::WriteRequest::Reader &message,
                                              riaps::ports::PortBase *port) {

        }

        void GpioDeviceComponent::OnClock(riaps::ports::PortBase *port) {

        }

        void GpioDeviceComponent::OnOneShotTimer(const std::string &) {

        }

        GpioDeviceComponent::~GpioDeviceComponent() {

        }
    }
}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    auto result = new gpiotoggleexample::components::GpioDeviceComponent(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
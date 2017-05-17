//
// Created by istvan on 5/16/17.
//

#include <ToggleGpioComponent.h>

namespace gpiotoggleexample {
    namespace components {

        ToggleGpioComponent::ToggleGpioComponent(_component_conf_j &config, riaps::Actor &actor)
            : ToggleGpioComponentBase(config, actor){
            _currentPid = getpid();
            _value = 0;
            std::cout << "ToggleGpioComponent: " << _currentPid << " starting" << std::endl;
        }

        void ToggleGpioComponent::OnToggle(riaps::ports::PortBase *port) {
            std::cout << "OnToggle()[" << _currentPid << "]"<< std::endl;
            _value = _value?0:1;

            capnp::MallocMessageBuilder messageBuilder;
            auto msgWriteRequest = messageBuilder.initRoot<messages::WriteRequest>();
            msgWriteRequest.setValue(_value);
            SendWriteGpioValue(messageBuilder, msgWriteRequest);
            std::cout << "OnToggle()[" << _currentPid << "]: send write request, setValue=" << _value << std::endl;
        }

        void ToggleGpioComponent::OnReadValue(riaps::ports::PortBase *port) {
            std::cout << "OnReadValue()[" << _currentPid << "]" << std::endl;
            capnp::MallocMessageBuilder messageBuilder;
            auto msgReadRequest = messageBuilder.initRoot<messages::ReadRequest>();
            msgReadRequest.setMsg("Read");
            SendPollGpioValue(messageBuilder, msgReadRequest);
            std::cout << "OnReadValue()[" << _currentPid << "]: send read request" << std::endl;
        }

        void ToggleGpioComponent::OnCurrentGpioValue(const messages::DataValue::Reader &message,
                                                     riaps::ports::PortBase *port) {
            std::cout << "OnCurrentGpioValue()[" << _currentPid << "]: " << message.getValue() << std::endl;
        }

        void ToggleGpioComponent::OnOneShotTimer(const std::string &timerid) {

        }

        ToggleGpioComponent::~ToggleGpioComponent() {

        }
    }
}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    auto result = new gpiotoggleexample::components::ToggleGpioComponent(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
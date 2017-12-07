//
// Created by istvan on 5/16/17.
//

#ifndef RIAPS_CORE_TOGGLEGPIOCOMPONENT_H
#define RIAPS_CORE_TOGGLEGPIOCOMPONENT_H

#include <base/ToggleGpioComponentBase.h>

namespace gpiotoggleexample{
    namespace components{
        class ToggleGpioComponent : public ToggleGpioComponentBase{
        public:
            ToggleGpioComponent(_component_conf &config, riaps::Actor &actor);

            void OnToggle(riaps::ports::PortBase *port);

            void OnReadValue(riaps::ports::PortBase *port);

            void OnCurrentGpioValue(const messages::DataValue::Reader &message,
                                            riaps::ports::PortBase *port);

            void OnOneShotTimer(const std::string &timerid);

            ~ToggleGpioComponent();

        private:
            __pid_t _currentPid;
            std::string _value;
        };
    }
}

extern "C" riaps::ComponentBase* create_component(_component_conf&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);

#endif //RIAPS_CORE_TOGGLEGPIOCOMPONENT_H

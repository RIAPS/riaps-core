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
            ToggleGpioComponent(_component_conf_j &config, riaps::Actor &actor);

            void OnToggle(riaps::ports::PortBase *port);

            void OnReadValue(riaps::ports::PortBase *port);

            void OnCurrentGpioValue(const messages::DataValue::Reader &message,
                                            riaps::ports::PortBase *port);

            void OnOneShotTimer(const std::string &timerid);

            ~ToggleGpioComponent();

        private:
            __pid_t _currentPid;
            uint8_t _value;
        };
    }
}

#endif //RIAPS_CORE_TOGGLEGPIOCOMPONENT_H

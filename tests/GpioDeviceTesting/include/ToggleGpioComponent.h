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
            ToggleGpioComponent();
            ~ToggleGpioComponent();
        };
    }
}

#endif //RIAPS_CORE_TOGGLEGPIOCOMPONENT_H

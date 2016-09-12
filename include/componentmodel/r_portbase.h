//
// Created by parallels on 9/6/16.
//

#ifndef RIAPS_R_PORTBASE_H
#define RIAPS_R_PORTBASE_H

#include "r_configuration.h"
#include "r_discoverdapi.h"

#include <iostream>

namespace riaps {
    class PortBase {

    public:
        PortBase();

        std::string GetInterfaceAddress(std::string ifacename);

        ~PortBase();

    };
}

#endif //RIAPS_R_PORTBASE_H

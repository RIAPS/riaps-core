//
// Created by istvan on 5/11/17.
//

#ifndef RIAPS_CORE_R_PERIPHERAL_H
#define RIAPS_CORE_R_PERIPHERAL_H

#include <componentmodel/r_actor.h>

namespace riaps {

    class Actor;

    class Peripheral {
    public:
        Peripheral(riaps::Actor* parentActor);
        ~Peripheral();

        void Setup(const std::string &appName,
                   const std::string &modelName,
                   const std::string &typeName,
                   const std::vector<std::pair<std::string, std::string>> &args);

        enum STATE { STARTING, INITIAL, READY, ACTIVE, CHECKPOINTING, INACTIVE, PASSIVE, DESTROYED};

    protected:
        STATE _state;
        riaps::Actor* _parentActor;
    };
}

#endif //RIAPS_CORE_R_PERIPHERAL_H

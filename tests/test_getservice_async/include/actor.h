//
// Created by parallels on 9/21/16.
//

#ifndef RIAPS_FW_ACTOR_H
#define RIAPS_FW_ACTOR_H

#include "componentmodel/r_actor.h"
#include "componentmodel/r_componentbase.h"
#include <czmq.h>
#include <string>

class actor : public riaps::Actor {

public:

    actor(std::string actorid);
    void start();
};

#endif //RIAPS_FW_ACTOR_H

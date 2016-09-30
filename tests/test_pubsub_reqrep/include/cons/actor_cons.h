//
// Created by istvan on 9/30/16.
//

#ifndef RIAPS_FW_ACTOR_CONS_H
#define RIAPS_FW_ACTOR_CONS_H

#include "comp_sub.h"
#include "comp_rep.h"

#include "componentmodel/r_actor.h"
#include "componentmodel/r_componentbase.h"
#include <czmq.h>
#include <string>

class actor : public riaps::Actor {

public:

    actor(std::string actorid);
    virtual void start();
};

#endif //RIAPS_FW_ACTOR_CONS_H

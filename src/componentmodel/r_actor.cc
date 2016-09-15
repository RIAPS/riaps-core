//
// Created by parallels on 9/6/16.
//


#include "componentmodel/r_actor.h"


namespace riaps {
    riaps::Actor::Actor() {
        actor_zsock = zsock_new_rep("tcp://*:!");
        assert(actor_zsock);
        poller = zpoller_new(actor_zsock, NULL);
        assert(poller);

        register_actor("ActorSub");
    }

    void riaps::Actor::start(){



    }

    riaps::Actor::~Actor() {
        deregister_actor("ActorSub");
        zpoller_destroy(&poller);
        zsock_destroy(&actor_zsock);
    }
}



//namespace riaps{

    //Actor::Actor() {}

    //Actor::Actor(std::string configfile){
    //}

    //Actor::~Actor(){}
//}
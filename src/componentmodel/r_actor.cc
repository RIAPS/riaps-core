//
// Created by parallels on 9/6/16.
//


#include "componentmodel/r_actor.h"


namespace riaps {
    riaps::Actor::Actor(std::string actorid):_actor_id(actorid) {
        _actor_zsock = zsock_new_rep("tcp://*:!");
        assert(_actor_zsock);
        _poller = zpoller_new(_actor_zsock, NULL);
        assert(_poller);

        register_actor(_actor_id);
    }

    void riaps::Actor::start(){



    }

    riaps::Actor::~Actor() {
        deregister_actor(_actor_id);
        zpoller_destroy(&_poller);
        zsock_destroy(&_actor_zsock);
    }
}



//namespace riaps{

    //Actor::Actor() {}

    //Actor::Actor(std::string configfile){
    //}

    //Actor::~Actor(){}
//}
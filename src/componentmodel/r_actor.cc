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

        //_actor_comp_proxy = zactor_new (zproxy, NULL);
        //assert(_actor_comp_proxy);
        //zstr_sendx (_actor_comp_proxy, "FRONTEND", "PULL", "inproc://frontend", NULL);
        //zsock_wait (_actor_comp_proxy);
        //zstr_sendx (_actor_comp_proxy, "BACKEND", "PUSH", "inproc://backend", NULL);
        //zsock_wait (_actor_comp_proxy);

        register_actor(_actor_id);
    }

    void riaps::Actor::start(){

    }

    riaps::Actor::~Actor() {
        deregister_actor(_actor_id);
        zsock_destroy(&_actor_zsock);
        zpoller_destroy(&_poller);
        //zactor_destroy(&_actor_comp_proxy);
    }
}



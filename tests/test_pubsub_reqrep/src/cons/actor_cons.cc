//
// Created by istvan on 9/30/16.
//

#include "cons/actor_cons.h"

actor::actor(std::string actorid) : riaps::Actor(actorid) {

}

void actor::start() {
    component_conf cconf;

    subscriber_conf sconf;
    sconf.remoteservice_name = "GpsPublisher";
    sconf.servicename = "GpsSubscriber";       // Optional parameter
    cconf.subscribers_config.push_back(sconf);

    // Todo add request port

    component_sub c(cconf);


    while (!zsys_interrupted) {
        void *which = zpoller_wait(_poller, 2000);

        if (which == _actor_zsock) {
            // Actor commands
        } else {

        }
    }
}
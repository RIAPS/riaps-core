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

    request_conf rconf;
    rconf.remoteservice_name = "GpsSinkerService";
    cconf.requests_config.push_back(rconf);

    // Todo add request port

    component_sub forwarder (cconf);


    response_conf resp_conf;
    resp_conf.network_iface ="enp0s5";
    resp_conf.port = 0; // auto bind
    resp_conf.servicename = "GpsSinkerService";

    component_conf sinkerconf;
    sinkerconf.responses_config.push_back(resp_conf);


    component_rep sinker(sinkerconf);

    while (!zsys_interrupted) {
        void *which = zpoller_wait(_poller, 2000);

        if (which == _actor_zsock) {
            // Actor commands
        } else {

        }
    }
}
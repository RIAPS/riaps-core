//
// Created by istvan on 9/30/16.
//

#include "prod/actor_prod.h"


actor::actor(std::string actorid) : riaps::Actor(actorid) {

}

void actor::start() {
    component_conf cconf;

    // Publisher port
    publisher_conf pport;
    pport.servicename = "GpsPublisher";
    pport.network_iface = "enp0s5";     // Ethernet iface to be used
    pport.port = 0;                     // Auto binding

    // Periodic timer
    periodic_timer_conf ptimer;
    ptimer.timerid   = "GpsTimer";
    ptimer.interval  = 3000;       //millisec

    cconf.component_name = "ComponentGpsPub";
    cconf.publishers_config.push_back(pport);
    cconf.periodic_timer_config.push_back(ptimer);

    // Starts GPS component
    component_gps cgps(cconf);

    while (!zsys_interrupted) {
        void *which = zpoller_wait(_poller, 2000);

        if (which == _actor_zsock) {
            // Maybe later, control messages to the actor
        } else {
        }
    }
}
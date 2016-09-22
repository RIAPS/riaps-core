//
// Created by parallels on 9/21/16.
//

#include "actor.h"
#include "component_pub.h"


actor::actor(std::string actorid) : riaps::Actor(actorid) {

}

void actor::start() {
    component_conf cconf;

    publisher_conf pport;
    pport.servicename = "Publisher1";
    pport.network_iface = "eth1";
    pport.port = 0;            // Auto binding

    cconf.component_name = "ComponentPub";
    cconf.publishers_config.push_back(pport);

    subscriber_conf sconf1;
    sconf1.remoteservice_name = "Publisher1";
    sconf1.servicename = "Subscriber1";
    cconf.subscribers_config.push_back(sconf1);

    component_pub c(cconf);

    int i = 0;

    //int counter =0;

    while (!zsys_interrupted) {
        //if (counter++==5){
        //    int pid = getpid();
        //    kill(pid, SIGINT);
        //}


        void *which = zpoller_wait(_poller, 2000);

        if (which == _actor_zsock) {
            // Maybe later
        } else {

            // Sends test message

            //std::vector <std::string> params;

            //params.push_back("kukucs");

            //zmsg_t *msg = create_message(MSG_ACT_PING, params);

            //c.GetPublisherPorts()[0]->PublishMessage(&msg);
        }
    }
}

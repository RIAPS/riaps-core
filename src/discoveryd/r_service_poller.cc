//
// Created by parallels on 9/20/16.
//

#include "discoveryd/r_service_poller.h"

#define RETRY_PERIOD 4000

void
service_poller_actor (zsock_t *pipe, void *args) {

    zsock_t *poller_socket = zsock_new_rep("inproc://servicepoller");
    assert(poller_socket);

    zpoller_t *poller = zpoller_new(pipe, poller_socket, NULL);
    assert(poller);

    bool terminated = false;
    zsock_signal(pipe, 0);

    while (!terminated) {
        void *which = zpoller_wait(poller, RETRY_PERIOD);

        // Handling messages from the caller (e.g.: $TERM$)
        if (which == pipe) {
            zmsg_t *msg = zmsg_recv(which);
            if (!msg) {
                std::cout << "No msg => interrupted" << std::endl;
                break;
            }

            char *command = zmsg_popstr(msg);
            if(!command){

            }
        }
    }

    zsock_destroy(&poller_socket);
    zpoller_destroy(&poller);
}
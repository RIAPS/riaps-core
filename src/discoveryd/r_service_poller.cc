//
// Created by parallels on 9/20/16.
//

#include "discoveryd/r_service_poller.h"
#include <vector>

#define RETRY_PERIOD 4000 // millisec

void
execute_query(std::vector<service_query_params>& params) {
    std::vector<std::vector<service_query_params>::iterator> removables;

    for(auto it=params.begin(); it!=params.end(); it++)
    {
        std::vector<service_details> service_list;

        disc_getservicedetails(it->servicename, service_list);

        // Prepare the answer
        // Create a message frame for each service
        if (!service_list.empty()) {
            zmsg_t *response_msg = zmsg_new();

            for (auto it = service_list.begin(); it != service_list.end(); it++) {
                zmsg_t *submessage = zmsg_new();
                service_details_to_zmsg(*it, submessage);
                zmsg_addmsg(response_msg, &submessage);
            }

            zsock_t* replysocket = zsock_new_push(it->replyaddress.c_str());

            // TODO: if send was succesfull
            removables.push_back(it);
            zmsg_send(&response_msg, replysocket);


            zsock_destroy(&replysocket);
        }
    }

    for (auto removable : removables){
        params.erase(removable);
    }
}

void
service_poller_actor (zsock_t *pipe, void *args) {

    //zsock_t *poller_socket = zsock_new_rep("inproc://servicepoller");
    //assert(poller_socket);

    zpoller_t *poller = zpoller_new(pipe, NULL);
    assert(poller);

    bool terminated = false;
    zsock_signal(pipe, 0);

    std::vector<service_query_params> servicequeryparams;

    while (!terminated) {
        void *which = zpoller_wait(poller, RETRY_PERIOD);

        // Handling messages from the caller (e.g.: $TERM$)
        if (which == pipe) {
            //zmsg_t *msg = zmsg_recv(which);
            //if (!msg) {
            //    std::cout << "No msg => interrupted" << std::endl;
            //    break;
            //}

            char* servicename;
            char* replyaddress;
            zsock_recv(pipe, "ss", &servicename, &replyaddress);

            std::cout << "Add service to queue: " << servicename << " " << replyaddress << std::endl;

            service_query_params params;
            params.replyaddress = std::string(replyaddress);
            params.servicename  = std::string(servicename);

            servicequeryparams.push_back(params);
        }

        execute_query(servicequeryparams);
    }

    //zsock_destroy(&poller_socket);
    zpoller_destroy(&poller);
}
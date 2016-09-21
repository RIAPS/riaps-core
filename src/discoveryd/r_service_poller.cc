//
// Created by parallels on 9/20/16.
//

#include "discoveryd/r_service_poller.h"

#define RETRY_PERIOD 4000 // millisec

void
execute_query(std::vector<service_query_params>& params) {

    std::vector<service_query_params*> removable;

    //std::map<std::string, zsock_t*> replycache;

    for(auto current_param : params)
    {
        std::vector<service_details> service_list;

        disc_getservicedetails(current_param.servicename, service_list);

        // Prepare the answer
        // Create a message frame for each service

        if (!service_list.empty()) {
            zmsg_t *response_msg = zmsg_new();

            for (auto it = service_list.begin(); it != service_list.end(); it++) {
                zmsg_t *submessage = zmsg_new();
                service_details_to_zmsg(*it, submessage);
                zmsg_addmsg(response_msg, &submessage);
            }

            zsock_t* replysocket = zsock_new_push(current_param.replyaddress.c_str());

            removable.push_back(&current_param);
            zmsg_send(&response_msg, replysocket);

            zsock_destroy(&replysocket);
        }


        //Ask the details, if the service is registered, remove from the queue

        //std::cout << ' ' << myqueue.front();
        //myqueue.pop();
    }
}

void
service_poller_actor (zsock_t *pipe, void *args) {

    zsock_t *poller_socket = zsock_new_rep("inproc://servicepoller");
    assert(poller_socket);

    zpoller_t *poller = zpoller_new(pipe, poller_socket, NULL);
    assert(poller);

    bool terminated = false;
    zsock_signal(pipe, 0);

    std::vector<service_query_params> servicequeryparams;

    while (!terminated) {
        void *which = zpoller_wait(poller, RETRY_PERIOD);

        // Handling messages from the caller (e.g.: $TERM$)
        if (which == pipe) {
            zmsg_t *msg = zmsg_recv(which);
            if (!msg) {
                std::cout << "No msg => interrupted" << std::endl;
                break;
            }

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

    zsock_destroy(&poller_socket);
    zpoller_destroy(&poller);
}
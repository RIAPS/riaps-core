//
// Created by parallels on 9/20/16.
//

#include "discoveryd/r_service_poller.h"
#include <vector>

#define RETRY_PERIOD 4000 // millisec

// TODO: Maintain records with "old" timestamps (and define what "old" really means)
void
execute_query(std::vector<service_query_params>& params, zsock_t* asyncresponsesocket) {
    std::vector<std::vector<service_query_params>::iterator> removables;

    for(auto it=params.begin(); it!=params.end(); it++)
    {
        std::vector<service_details> service_list;

        disc_getservicedetails(it->servicename, service_list);

        // Prepare the answer
        // Create a message frame for each service
        if (!service_list.empty()) {
            std::cout << "Service list has " << service_list.size() << " elements." <<  std::endl;
            zmsg_t *response_msg = zmsg_new();

            // Add filter
            zframe_t* filterframe = zframe_new(it->replyaddress.c_str(), it->replyaddress.length());
            int rc = zmsg_prepend(response_msg, &filterframe);

            assert(rc==0);

            for (auto it = service_list.begin(); it != service_list.end(); it++) {
                zmsg_t *submessage = zmsg_new();
                service_details_to_zmsg(*it, submessage);
                zmsg_addmsg(response_msg, &submessage);
            }

            std::cout << "Send response, with filter: " << it->replyaddress << std::endl;
            //zsock_t* replysocket = zsock_new_req(it->replyaddress.c_str());
            //assert(replysocket);

            rc = zmsg_send(&response_msg, asyncresponsesocket);

            // If the message sent
            if (rc==0){
                //zmsg_recv(replysocket);
                removables.push_back(it);
            }
            //zclock_sleep(200);
            //zsock_destroy(&replysocket);
        }
        else {
            std::cout << "Service list is empty" << std::endl;
        }
    }

    for (auto removable : removables){
        params.erase(removable);
    }
}

void
service_poller_actor (zsock_t *pipe, void *args) {

    zpoller_t *poller = zpoller_new(pipe, NULL);
    assert(poller);

    zsock_t* asyncresponse = zsock_new_pub("ipc://asyncresponsepublisher");
    assert(asyncresponse);

    bool terminated = false;
    zsock_signal(pipe, 0);

    std::vector<service_query_params> servicequeryparams;

    // TODO: Handle $TERM to avoid dangling sockets
    while (!terminated) {
        void *which = zpoller_wait(poller, RETRY_PERIOD);

        // Handling messages from the caller (e.g.: $TERM$)
        if (which == pipe) {
            zmsg_t *msg = zmsg_recv(which);
            if (!msg) {
                std::cout << "No msg => interrupted" << std::endl;
                break;
            }

            char* command = zmsg_popstr(msg);
            if (streq(command, "$TERM")){
                std::cout << "$TERM arrived" << std::endl;
                terminated = true;
            } else {
                char* servicename = command;
                char* replyaddress;
                //zsock_recv(pipe, "ss", &servicename, &replyaddress);

                replyaddress = zmsg_popstr(msg);
                std::cout << "Add service to queue: " << servicename << " " << replyaddress << std::endl;

                service_query_params params;

                params.replyaddress = std::string(replyaddress);
                params.servicename  = std::string(servicename);
                params.timestamp    = zclock_mono();

                servicequeryparams.push_back(params);

                free(replyaddress);
            }

            free(command);
            zmsg_destroy(&msg);
        }

        execute_query(servicequeryparams, asyncresponse);
    }

    zsock_destroy(&asyncresponse);
    zpoller_destroy(&poller);
}
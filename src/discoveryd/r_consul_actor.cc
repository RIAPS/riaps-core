#include "discoveryd/r_consul_actor.h"

// TODO: merge with riaps actor
void consul_actor (zsock_t *pipe, void *args)
{
    zpoller_t* poller = zpoller_new(pipe, NULL);

    bool terminated = false;

    zsock_signal (pipe, 0);

    while (!terminated){
        void *which = zpoller_wait(poller, -1);

        if (which == pipe) {
            zmsg_t* msg = zmsg_recv(which);
            if (!msg){
                std::cout << "No msg => interrupted" << std::endl;
                break;
            }

            char* command = zmsg_popstr(msg);

            if (streq(command, "JOIN")) {
                std::cout << "JOIN arrived" << std::endl;
                bool has_more_msg = true;
                
                while (has_more_msg){
                    char* param = zmsg_popstr(msg);
                    if (param){
                        std::cout << "Connect to: " << param;
                        joinToCluster(std::string(param));
                        free(param);
                    } else{
                        has_more_msg = false;
                    }
                }
            } else
            if (streq(command, "$TERM")){
                std::cout << "$TERM arrived" << std::endl;
                terminated = true;
            }

            free(command);
            zmsg_destroy(&msg);

            std::cout << "Arrived something on PIPE" << std::endl;
        }
        else {
            std::cout << "Check, nothing arrived, but zpoller doesn't wait more" << std::endl; 
        }
    }

    zpoller_destroy(&poller);
}
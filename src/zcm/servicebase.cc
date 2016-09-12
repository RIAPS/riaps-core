
#include "servicebase.h"


#define PING_PERIOD 3000 //msec

namespace zcm{

    void service_actor (zsock_t *pipe, void *args) {
        zpoller_t* poller = zpoller_new(pipe, NULL);

        char* name = (char*)args;

        bool terminated = false;

        zsock_signal (pipe, 0);

        while (!terminated) {
            void *which = zpoller_wait(poller, PING_PERIOD);

            if (which == pipe) {
                zmsg_t *msg = zmsg_recv(which);
                if (!msg) {
                    std::cout << "No msg => interrupted" << std::endl;
                    break;
                }
                zmsg_destroy(&msg);
            }

            // Timeout, send PING to the discovery service
            else{
                std::cout << "PING SENT from " << name << std::endl;

                std::string strname(name);
                ping_service(strname);
            }
        }

        zpoller_destroy(&poller);
    }


    servicebase::servicebase(std::string name) : name(name){
        servicecheck = zactor_new(service_actor, (void*)name.c_str());
    };

    servicebase::~servicebase(){
        zactor_destroy(&servicecheck);
    };

    // Return the publisher name
    std::string servicebase::get_name() {
        return name;
    }
}
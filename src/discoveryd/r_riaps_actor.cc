#include "discoveryd/r_riaps_actor.h"
#include "utils/r_message.h"
#include "utils/r_utils.h"
#include "discoveryd/r_riaps_cmd_handler.h"
#include "discoveryd/r_service_poller.h"
#include <unistd.h>

#define REGULAR_MAINTAIN_PERIOD 3000 //msec

void
riaps_actor (zsock_t *pipe, void *args)
{
    std::srand(std::time(0));

    init_command_mappings();

    // Response socket for incoming messages from RIAPS Components
    zsock_t * riaps_socket = zsock_new_rep ("ipc://riapsdiscoveryservice");
    assert(riaps_socket);

    zactor_t* act_service_poller = zactor_new(service_poller_actor, NULL);
    assert(act_service_poller);

    zpoller_t* poller = zpoller_new(pipe, riaps_socket, act_service_poller, NULL);
    assert(poller);


    bool terminated = false;
    zsock_signal (pipe, 0);

    // Store the last checkins of the registered services
    // If there was no checkin in SERVICE_TIMEOUT ms then, remove the service from consul
    std::map<std::string, int64_t> service_checkins;

    char hostname[256];
    int hostnameresult = gethostname(hostname, 256);

    if (hostnameresult == -1) {
        std::cout << "hostname cannot be resolved" << std::endl;
        return;
    }

    disc_registernode(hostname);

    while (!terminated){
        void *which = zpoller_wait(poller, REGULAR_MAINTAIN_PERIOD);

        // Handling messages from the caller (e.g.: $TERM$)
        if (which == pipe) {
            zmsg_t* msg = zmsg_recv(which);
            if (!msg){
                std::cout << "No msg => interrupted" << std::endl;
                break;
            }

            char* command = zmsg_popstr(msg);

            if (streq(command, "$TERM")){
                std::cout << "R_ACTOR $TERM arrived, deregister node" << std::endl;

                disc_deregisternode(std::string(hostname));

                terminated = true;
            }
            //else if(streq(command, CMD_DISC_REGISTER_NODE)){
                // Register a node in the KV storage
            //    char* nodename = zmsg_popstr(msg);
            //    if (nodename) {
            //        std::string nodekey = "nodes/" + std::string(nodename);

                    //TODO: put ip address as value
            //        disc_registerkey(nodekey, "" );

             //       free(nodename);

             //   }
            //}

            free(command);
            zmsg_destroy(&msg);
        }

        // Handling messages from the RIAPS FW
        // Discovery service commands
        else if(which == riaps_socket){
            zmsg_t* msg = zmsg_recv(which);
            
            if (!msg){
                std::cout << "No msg => interrupted" << std::endl;
                break;
            }

            bool has_more_msg = true;
            char* command = zmsg_popstr(msg);
            
            if (command){

                // TODO: only handle_command should be called, without ifs
                if (streq(command, CMD_DISC_REGISTER_SERVICE)) {
                    bool handle_result = handle_command(command, msg, riaps_socket);
                    assert(handle_result);
                }
                else if (streq(command, CMD_DISC_DEREGISTER_SERVICE)){
                    bool handle_result = handle_command(command, msg, riaps_socket);
                    assert(handle_result);
                }
                else if (streq(command, CMD_DISC_GET_SERVICES)) {
                    std::vector<std::string> service_list;

                    // Collect all the registered services
                    disc_getservices(service_list);

                    // Prepare the answer (in ZMQ message)
                    zmsg_t* msg = zmsg_new();
                    zmsg_addstr(msg, MSG_SERVICE_LIST_HEADER);

                    // Add each servicename as an individual frame
                    for(auto it = service_list.begin(); it!=service_list.end(); it++) {
                        zmsg_addstr(msg, it->c_str());
                    }

                    // Send it!
                    zmsg_send(&msg, riaps_socket);
                }
                else if (streq(command, CMD_DISC_GETSERVICE_BY_NAME)) {
                    std::vector<service_details> service_list;
                    char* servicename_param = zmsg_popstr(msg);

                    if (servicename_param) {
                        std::string servicename(servicename_param);
                        disc_getservicedetails(servicename, service_list);
                        free(servicename_param);

                        // Prepare the answer
                        // Create a message frame for each service

                        // TODO: check is there a response in the list to send...
                        zmsg_t *response_msg = zmsg_new();

                        for (auto it = service_list.begin(); it != service_list.end(); it++) {
                            zmsg_t *submessage = zmsg_new();
                            service_details_to_zmsg(*it, submessage);
                            zmsg_addmsg(response_msg, &submessage);
                        }

                        zmsg_send(&response_msg, riaps_socket);
                    }
                    else{
                        // TODO send back empty message
                    }
                }
                else if (streq(command, CMD_DISC_GETSERVICE_BY_NAME_ASYNC)){
                    char* servicename_param = zmsg_popstr(msg);
                    if (servicename_param) {
                        char* returnaddress_param = zmsg_popstr(msg);
                        if (returnaddress_param){

                            // Pass the message to the ASYNC handler
                            zsock_send(act_service_poller, "ss", servicename_param, returnaddress_param);

                            free(returnaddress_param);
                        }
                        free(servicename_param);

                    }
                    else{
                        // TODO send back empty message
                    }
                    zstr_send(riaps_socket, "OK");
                }
                else if(streq(command, CMD_DISC_REGISTER_NODE)){
                    // Register a node in the KV storage
                    char* nodename = zmsg_popstr(msg);
                    if (nodename) {
                        std::string nodekey = "/nodes/" + std::string(nodename);

                        //TODO: put ip address as value
                        disc_registerkey(nodekey, "" );

                        free(nodename);
                        zstr_send(riaps_socket, "OK");
                    }
                }
                else if (streq(command, CMD_DISC_REGISTER_ACTOR)){
                    char* actorname = zmsg_popstr(msg);
                    if (actorname){
                        disc_registeractor(hostname, actorname);
                        free(actorname);
                        zstr_send(riaps_socket, "OK");
                    }
                }
                else if(streq(command, CMD_DISC_DEREGISTER_ACTOR)){
                    char* actorname = zmsg_popstr(msg);
                    if (actorname){
                        disc_deregisteractor(hostname, actorname);

                        free(actorname);

                        zstr_send(riaps_socket, "OK");
                    }
                }
                else if(streq(command, CMD_DISC_PING)){

                    std::cout << "Ping arrived" << std::endl;

                    char* service_name = zmsg_popstr(msg);
                    if (service_name){

                        //std::cout << "   " << service_name << std::endl;
                        int64_t time = zclock_mono();
                        service_checkins[service_name] = time;

                        // remove outdated services from the cache and from the discovery service
                        auto outdateds = maintain_servicecache(service_checkins);

                        for (auto outdated : outdateds){
                            std::cout << outdated << ";" ;
                        }

                        std::cout << std::endl;

                        free(service_name);
                        //zstr_send("OK", riaps_socket);
                        zstr_send(riaps_socket, "OK");
                    }
                }
                else {

                }

                free(command);
            }
        }
        else {
            std::cout << "Regular maintain, cannot stop: " << terminated <<std::endl;

            //zmsg_t* msg = zmsg_recv(which);
            //if (msg){
            //    std::cout << "HEYY MSG!" << std::endl;
            //    break;
            //}

            //auto outdateds = maintain_servicecache(service_checkins);

            //for (auto outdated : outdateds){
            //    std::cout << outdated << ";" ;
            //}

            //std::cout << std::endl;
        }
    }


    zpoller_destroy(&poller);
    zactor_destroy(&act_service_poller);
    zsock_destroy(&riaps_socket);
}


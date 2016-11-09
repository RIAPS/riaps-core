#include "discoveryd/r_riaps_actor.h"
#include "utils/r_message.h"
#include "utils/r_utils.h"
#include "discoveryd/r_riaps_cmd_handler.h"
#include "discoveryd/r_service_poller.h"
#include <unistd.h>
#include <capnp/common.h>

#define REGULAR_MAINTAIN_PERIOD 3000 //msec

void
riaps_actor (zsock_t *pipe, void *args)
{
    std::srand(std::time(0));

    init_command_mappings();

    // Response socket for incoming messages from RIAPS Components
    zsock_t * riaps_socket = zsock_new_rep ("ipc://riapsdiscoveryservice");
    //zsock_t * riaps_socket = zsock_new_router ("ipc://riapsdiscoveryservice");
    assert(riaps_socket);

    zactor_t* async_service_poller = zactor_new(service_poller_actor, NULL);
    assert(async_service_poller);

    zpoller_t* poller = zpoller_new(pipe, riaps_socket, async_service_poller, NULL);
    assert(poller);

    // Pair sockets for Actor communication
    std::map<std::string, actor_details> actor_pair_details;


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

            zframe_t* capnp_msgbody = zmsg_pop(msg);
            size_t    size = zframe_size(capnp_msgbody);
            byte*     data = zframe_data(capnp_msgbody);

            auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

            capnp::FlatArrayMessageReader reader(capnp_data);
            auto msg_discoreq= reader.getRoot<DiscoReq>();

            // Register actor
            if (msg_discoreq.which() == DiscoReq::ACTOR_REG) {
                auto msg_actorreq = msg_discoreq.getActorReg();
                auto actorname    = std::string(msg_actorreq.getActorName());
                auto appname      = std::string(msg_actorreq.getAppName());

                std::string clientKeyBase = "/" + appname + '/' + actorname + "/";

                // If the actor already registered
                if (actor_pair_details.find(clientKeyBase)!=actor_pair_details.end()) {
                    // TODO: What to do then?
                } else{
                    // Open a new PAIR socket for actor communication
                    zsock_t * actor_socket = zsock_new (ZMQ_PAIR);
                    auto port = zsock_bind(actor_socket, "tcp://*:!");

                    actor_pair_details[clientKeyBase] = actor_details{};
                    actor_pair_details[clientKeyBase].socket     = actor_socket;
                    actor_pair_details[clientKeyBase].globalport = port;
                    actor_pair_details[clientKeyBase].localport  = port;


                    // Create and send the Response
                    capnp::MallocMessageBuilder message;
                    auto drepmsg = message.initRoot<DiscoRep>();
                    auto arepmsg = drepmsg.initActorReg();

                    arepmsg.setPort(port);
                    arepmsg.setStatus(Status::OK);

                    auto serializedMessage = capnp::messageToFlatArray(message);

                    zmsg_t* msg = zmsg_new();
                    zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

                    zmsg_send(&msg, riaps_socket);



                    // Unique id, TODO: Replace it with MacAddress
                    //auto uuid = zuuid_new();
                    //auto uuid_str = std::string(zuuid_str(uuid));
                    //zuuid_destroy(&uuid);

                    //auto clientKeyLocal = clientKeyBase + uuid_str;
                    //self.clients[clientKeyLocal] = port

                    //clientKeyGlobal = clientKeyBase + self.hostAddress
                    //self.clients[clientKeyGlobal] = port



                }
            }




            //capnp::initMessageBuilderFromFlatArrayCopy(reader.getRoot(), message);

            //capnp::MallocMessageBuilder message;
            //capnp::initMessageBuilderFromFlatArrayCopy(kjptr, message);
            /*
            char* command = zmsg_popstr(msg);
            
            if (command){

                if (streq(command, CMD_DISC_GETSERVICE_BY_NAME_POLL_ASYNC)) {
                    //zframe_t* flat_msgbody = zmsg_pop(msg);
                    //byte* framedata = zframe_data(flat_msgbody);
                    //auto m = Getmsg_getservice_poll_request(framedata);

                    //auto s = m->service_name()->str();
                    //auto r = m->reply_id()->str();


                    //int i =5;
                }

                    // TODO: only handle_command should be called, without ifs
                if (streq(command, CMD_DISC_REGISTER_SERVICE)) {
                    bool handle_result = handle_command(command, msg, riaps_socket, async_service_poller);
                    assert(handle_result);
                }
                else if (streq(command, CMD_DISC_DEREGISTER_SERVICE)){
                    bool handle_result = handle_command(command, msg, riaps_socket, async_service_poller);
                    assert(handle_result);
                }
                else if (streq(command, CMD_DISC_GET_SERVICES)) {
                    bool handle_result = handle_command(command, msg, riaps_socket, async_service_poller);
                    assert(handle_result);
                }
                else if (streq(command, CMD_DISC_GETSERVICE_BY_NAME)) {
                    bool handle_result = handle_command(command, msg, riaps_socket, async_service_poller);
                    assert(handle_result);
                }
                else if (streq(command, CMD_DISC_GETSERVICE_BY_NAME_ASYNC)){
                    bool handle_result = handle_command(command, msg, riaps_socket, async_service_poller);
                    assert(handle_result);
                }
                else if(streq(command, CMD_DISC_REGISTER_NODE)){
                    bool handle_result = handle_command(command, msg, riaps_socket, async_service_poller);
                    assert(handle_result);
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
            }*/
        }
        else {
            //std::cout << "Regular maintain, cannot stop: " << terminated <<std::endl;

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

    for (auto actor_pair_detail : actor_pair_details){
        if (actor_pair_detail.second.socket!=NULL) {
            zsock_destroy(&actor_pair_detail.second.socket);
            actor_pair_detail.second.socket=NULL;
        }
    }


    zpoller_destroy(&poller);
    zactor_destroy(&async_service_poller);
    zsock_destroy(&riaps_socket);
}


//
// Created by istvan on 11/11/16.
//

#include "loggerd/r_loggerd.h"

/*
void
logger_actor (zsock_t *pipe, void *args)
{

    //zsock_t * riaps_socket = zsock_new_router ("ipc://riapsdiscoveryservice");
    assert(logger_socket);

    bool terminated = false;
    zsock_signal (pipe, 0);

    zpoller_t* poller = zpoller_new(logger_socket, NULL);

    while (!terminated) {
        void *which = zpoller_wait(poller, 1000);

        if (which == pipe) {
            zmsg_t *msg = zmsg_recv(which);
            if (!msg) {
                std::cout << "No msg => interrupted" << std::endl;
                break;
            }

            char* log_str = zmsg_popstr(msg);

            if (streq(log_str, "$TERM")){
                std::cout << "Logger terminates." << std::endl;

                terminated = true;
            }
            else{
                std::cout << log_str << std::endl;
            }

            delete log_str;
        }
    }

    zsock_destroy(&logger_socket);

}
*/

int main()
{
    // Response socket for incoming messages from RIAPS Components
    // We should use REP, becuase we can detect connection timeout (logger not running)
    // Now pull is used, no response to the clients.
    zsock_t* cmd_socket = zsock_new_pull (RIAPS_LOG_CHANNEL);
    zsock_t* log_socket = zsock_new(ZMQ_SUB);
    zsock_set_subscribe(log_socket, "");

    zpoller_t* poller = zpoller_new(cmd_socket, log_socket, NULL);

    while (!zsys_interrupted){
        auto which = zpoller_wait(poller, 1000);

        // Incoming control message
        if (which == cmd_socket) {
            zmsg_t* msg = zmsg_recv(which);

            if (msg != NULL){
                auto capnp_data = zmsg_to_arrayPtr(msg);
                capnp::FlatArrayMessageReader reader(capnp_data);

                auto logitem = reader.getRoot<Log>();

                if (logitem.isNewLogPublisher()){
                    auto newpublisher_msg = logitem.getNewLogPublisher();

                    //std::string host = newpublisher_msg.getSocket().getHost();
                    //auto port = newpublisher_msg.getSocket().getPort();

                    //std::string endpoint = host + std::to_string(port);

                    std::string endpoint = newpublisher_msg.getEndpoint();

                    zsock_connect(log_socket, endpoint.c_str());
                } else{
                    throw std::runtime_error("Invalid command, logger waits NewLogPublisher message.");
                }

                //delete capnp_data.begin();
                zmsg_destroy(&msg);
            }


            //zframe_t* capnp_msgbody = zmsg_pop(msg);
            //size_t    size = zframe_size(capnp_msgbody);
            //byte*     data = zframe_data(capnp_msgbody);

            //auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

            //delete data;

            // Destroys the inner frames too does it destroy capnpdata????
            //zmsg_destroy(&msg);

            //auto capnp_data = zmsg_to_arrayPtr(msg);

            //delete capnp_data.begin();

            //capnp::FlatArrayMessageReader reader(capnp_data);
            //auto msg_discoreq= reader.getRoot<DiscoReq>();
        }

        // Incoming log message
        else if (which == log_socket){
            zmsg_t* msg = zmsg_recv(which);

            if (msg != NULL){
                auto capnp_data = zmsg_to_arrayPtr(msg);
                capnp::FlatArrayMessageReader reader(capnp_data);

                auto logitem = reader.getRoot<Log>();

                if (logitem.isLogMessage()){
                    auto log_msg = logitem.getLogMessage();
                    std::string message = log_msg.getMessage();
                    auto log_type  = log_msg.getLogMessageType();

                    std::string log_type_str;

                    if (log_type == LogMessageType::INFO){
                        log_type_str = "info";
                    } else if (log_type == LogMessageType::DEBUG){
                        log_type_str = "debug";
                    } else if (log_type == LogMessageType::ERROR){
                        log_type_str = "error";
                    } else if (log_type == LogMessageType::NOTICE){
                        log_type_str = "notice";
                    } else if (log_type == LogMessageType::WARNING){
                        log_type_str = "warning";
                    }

                    std::cout << "[" << log_type_str << "] " << message << std::endl;
                    std::flush(std::cout);

                } else{
                    throw std::runtime_error("Invalid command, logger waits NewLogPublisher message.");
                }

                //delete capnp_data.begin();
                zmsg_destroy(&msg);
            }
        }

        // Poller idle, maintenance
        else if (which = NULL){

        }
    }

    zpoller_destroy(&poller);
    zsock_destroy(&cmd_socket);

    /*zactor_t* l_actor = zactor_new (logger_actor, NULL);
    zpoller_t* poller = zpoller_new(l_actor, NULL);

    while (!zsys_interrupted) {
        void* which = zpoller_wait(poller, 1000);
    }

    zpoller_destroy(&poller);
    zactor_destroy(&l_actor);*/

   /* /// Initialize ZLOG logger
    zsys_set_logident ("riaps discovery log");
    zsys_set_logsender (RIAPS_LOG_DISCOVERY);

    void *logger = zsys_socket (ZMQ_SUB, NULL, 0);
    assert (logger);
    int rc = zmq_connect (logger, RIAPS_LOG_DISCOVERY);
    assert (rc == 0);
    rc = zmq_setsockopt (logger, ZMQ_SUBSCRIBE, "", 0);
    assert (rc == 0);

    zsock_set_rcvtimeo(logger, 1000);

    while (!zsys_interrupted){
        char *received = zstr_recv (logger);

        if (received!=NULL) {
            std::cout << "Alma";
            std::cout << received << std::endl << std::flush;

            assert (received);
            zstr_free(&received);
        }

    }

    zsys_close(logger, NULL, 0);*/

    return 0;
}
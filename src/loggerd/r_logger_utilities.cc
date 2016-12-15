//
// Created by istvan on 11/13/16.
//

#include "loggerd/r_logger_utilities.h"


void sendLogMessage(zsock_t* log_socket, std::string logmessage, LogMessageType messageType){
    /////
    /// Request
    /////
    capnp::MallocMessageBuilder builder;
    auto logBuilder = builder.initRoot<Log>();
    auto logMessageBuilder = logBuilder.initLogMessage();

    logMessageBuilder.setMessage(logmessage);
    logMessageBuilder.setLogMessageType(messageType);

    auto serializedMessage = capnp::messageToFlatArray(builder);

    zmsg_t* msg = zmsg_new();
    zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

    zmsg_send(&msg, log_socket);
}

zsock_t* initLogger(){

    zsock_t* pub_socket = zsock_new_pub("tcp://127.0.0.1:!");
    auto pub_endpoint = zsock_endpoint(pub_socket);

    /////
    /// Request
    /////
    capnp::MallocMessageBuilder builder;
    auto logBuilder = builder.initRoot<Log>();
    auto newPublisherBuilder = logBuilder.initNewLogPublisher();

    newPublisherBuilder.setEndpoint(pub_endpoint);

    auto serializedMessage = capnp::messageToFlatArray(builder);

    zmsg_t* msg = zmsg_new();
    zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());

    zsock_t * client = zsock_new_push (RIAPS_LOG_CHANNEL);
    assert(client);

    zmsg_send(&msg, client);
    zclock_sleep (200);
    return pub_socket;
}
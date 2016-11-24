#include "componentmodel/r_discoverdapi.h"



bool register_service(std::string              app_name     ,
                      std::string              message_type ,
                      std::string              ip_address   ,
                      uint16_t                 port         ,
                      Kind                     kind         ,
                      Scope                    scope        ,
                      std::vector<std::string> tags
                      ) {

    bool result = false;

    /////
    /// Request
    /////
    capnp::MallocMessageBuilder message;
    auto dreqBuilder = message.initRoot<DiscoReq>();
    auto sreqBuilder = dreqBuilder.initServiceReg();

    auto sreqpath   = sreqBuilder.initPath();
    auto sreqsocket = sreqBuilder.initSocket();

    sreqpath.setAppName(app_name);
    sreqpath.setMsgType(message_type);
    sreqpath.setKind(kind);
    sreqpath.setScope(scope);

    sreqsocket.setHost(ip_address);
    sreqsocket.setPort(port);

    auto serializedMessage = capnp::messageToFlatArray(message);

    zmsg_t* msg = zmsg_new();
    zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());


    zsock_t * client = zsock_new_req (DISCOVERY_SERVICE_IPC);
    assert(client);

    zmsg_send(&msg, client);

    /////
    /// Response
    /////
    zmsg_t* msg_response = zmsg_recv(client);

    zframe_t* capnp_msgbody = zmsg_pop(msg_response);
    size_t    size = zframe_size(capnp_msgbody);
    byte*     data = zframe_data(capnp_msgbody);

    auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

    capnp::FlatArrayMessageReader reader(capnp_data);
    auto msg_discoreq= reader.getRoot<DiscoRep>();


    // Register actor
    if (msg_discoreq.isServiceReg()) {
        auto msg_response = msg_discoreq.getServiceReg();
        auto status = msg_response.getStatus();

        // TODO: Check status
        if (status == Status::OK){
            result = true;
        }
    }

    /////
    /// Clean up
    /////

    zframe_destroy(&capnp_msgbody);
    zmsg_destroy(&msg_response);
    zsock_destroy(&client);

    return result;

    /*
   // Create ZMQ message for RIAPS discovery service 
   // Create frames:
   zmsg_t* msg = zmsg_new();
   zmsg_addstr(msg, CMD_DISC_REGISTER_SERVICE);

    // Id and the name are the same now
   zmsg_addstr(msg, message_type.c_str());
   zmsg_addstr(msg, message_type.c_str());
   zmsg_addstr(msg, ip_address.c_str());
   zmsg_addstr(msg, port.c_str());

    // Todo: Indo separate frame (as a submessage)
   zmsg_addstr(msg, "$TAGS$");

   // Adding tag frames
   for (auto it = tags.begin(); it!=tags.end(); it++){
       zmsg_addstr(msg, it->c_str());
   }

   // Create the socket and send the register command
   zsock_t * client = zsock_new_req (DISCOVERY_SERVICE_IPC);
   assert(client);

   // TODO check return value
   zmsg_send(&msg, client);

   // Wait for the OK response
   // TODO: Specify OK response, error handling
   auto result = zstr_recv(client);
   std::cout << result;
   // TODO: if (result == somethingsomething) {}

   // release ZMQ socket
   zsock_destroy(&client);*/
}



std::vector<service_lookup_result>
subscribe_to_service(std::string app_name  ,
                     std::string part_name , // instance_name
 //                       std::string part_type ,
                     Kind        kind      ,
                     Scope       scope     ,
                     std::string port_name ,
                     std::string msg_type  // PortType
        ){

    std::vector<service_lookup_result> result;

    /////
    /// Request
    /////
    capnp::MallocMessageBuilder message;
    auto dreqBuilder = message.initRoot<DiscoReq>();
    auto slookupBuilder = dreqBuilder.initServiceLookup();

    auto pathBuilder = slookupBuilder.initPath();
    auto clientBuilder = slookupBuilder.initClient();

    pathBuilder.setAppName(app_name);
    pathBuilder.setMsgType(msg_type);
    pathBuilder.setKind(kind);
    pathBuilder.setScope(scope);

    clientBuilder.setInstanceName(part_name);
    clientBuilder.setPortName(port_name);

    auto serializedMessage = capnp::messageToFlatArray(message);

    zmsg_t* msg = zmsg_new();
    zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());


    zsock_t * client = zsock_new_req (DISCOVERY_SERVICE_IPC);
    assert(client);

    zmsg_send(&msg, client);

    /////
    /// Response
    /////
    zmsg_t* msg_response = zmsg_recv(client);

    zframe_t* capnp_msgbody = zmsg_pop(msg_response);
    size_t    size = zframe_size(capnp_msgbody);
    byte*     data = zframe_data(capnp_msgbody);

    auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

    capnp::FlatArrayMessageReader reader(capnp_data);
    auto msg_discoreq= reader.getRoot<DiscoRep>();


    // Register actor
    if (msg_discoreq.isServiceLookup()) {
        auto msg_service_lookup = msg_discoreq.getServiceLookup();
        auto status = msg_service_lookup.getStatus();
        auto sockets = msg_service_lookup.getSockets();

        if (status == Status::OK) {
            for (Socket::Reader socket : msg_service_lookup.getSockets()) {
                service_lookup_result result_item;

                result_item.host_name = socket.getHost();
                result_item.port      = socket.getPort();
                result_item.part_name = part_name;
                result_item.port_name = port_name;

                result.push_back(result_item);
            }
        }
    }

    /////
    /// Clean up
    /////

    zframe_destroy(&capnp_msgbody);
    zmsg_destroy(&msg_response);
    zsock_destroy(&client);

    return result;

}


zsock_t*
register_actor(std::string appname, std::string actorname){

    /////
    /// Request
    /////
    capnp::MallocMessageBuilder message;
    DiscoReq::Builder dreqBuilder = message.initRoot<DiscoReq>();
    ActorRegReq::Builder areqBuilder = dreqBuilder.initActorReg();

    areqBuilder.setActorName(actorname);
    areqBuilder.setAppName(appname);
    areqBuilder.setVersion("0");

    auto serializedMessage = capnp::messageToFlatArray(message);

    zmsg_t* msg = zmsg_new();
    zmsg_pushmem(msg, serializedMessage.asBytes().begin(), serializedMessage.asBytes().size());


    zsock_t * client = zsock_new_req (DISCOVERY_SERVICE_IPC);
    assert(client);

    zmsg_send(&msg, client);

    /////
    /// Response
    /////
    zmsg_t* msg_response = zmsg_recv(client);

    zframe_t* capnp_msgbody = zmsg_pop(msg_response);
    size_t    size = zframe_size(capnp_msgbody);
    byte*     data = zframe_data(capnp_msgbody);

    auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

    capnp::FlatArrayMessageReader reader(capnp_data);
    auto msg_discoreq= reader.getRoot<DiscoRep>();

    zsock_t* discovery_port = NULL;

    // Register actor
    if (msg_discoreq.isActorReg()) {
        auto msg_response = msg_discoreq.getActorReg();
        auto status = msg_response.getStatus();
        auto port = msg_response.getPort();


        // TODO: Check status

        if (status == Status::ERR){
            std::cout << "Couldn't register actor: " << actorname << std::endl;
        }
        else {
            auto discovery_endpoint = "tcp://localhost:" + std::to_string(port);
            discovery_port = zsock_new_pair(discovery_endpoint.c_str());
            assert(discovery_port);
        }
    }

    /////
    /// Clean up
    /////

    zsock_disconnect(client, DISCOVERY_SERVICE_IPC);
    zframe_destroy(&capnp_msgbody);
    zmsg_destroy(&msg_response);
    zsock_destroy(&client);

    return discovery_port;

    //if (!msg_response){
    //    std::cout << "No msg => interrupted" << std::endl;
    //    return false;
    //}


    /*zmsg_t* msg = zmsg_new();
    zmsg_addstr(msg, CMD_DISC_REGISTER_ACTOR);
    zmsg_addstr(msg, actorname.c_str());
    zsock_t * client = zsock_new_req (DISCOVERY_SERVICE_IPC);
    assert(client);

    // TODO check return value
    zmsg_send(&msg, client);

    char* msg_response = zstr_recv(client);

    if (!msg_response){
        std::cout << "No msg => interrupted" << std::endl;
        return;
    }
    else{
        free(msg_response);
    }

    zsock_destroy(&client);*/
}

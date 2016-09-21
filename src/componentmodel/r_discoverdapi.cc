#include "componentmodel/r_discoverdapi.h"


bool register_service(std::string              service_id   ,
                      std::string              service_name ,
                      std::string              ip_address   ,
                      std::string              port         ,
                      std::vector<std::string> tags
                      ) {

   // Create ZMQ message for RIAPS discovery service 
   // Create frames:
   zmsg_t* msg = zmsg_new();
   zmsg_addstr(msg, CMD_DISC_REGISTER_SERVICE);
   zmsg_addstr(msg, service_id.c_str());
   zmsg_addstr(msg, service_name.c_str());
   zmsg_addstr(msg, ip_address.c_str());
   zmsg_addstr(msg, port.c_str());

    // Todo: Indo separate frame (as a submessage)
   zmsg_addstr(msg, "$TAGS$");

   // Adding tag frames
   for (auto it = tags.begin(); it!=tags.end(); it++){
       zmsg_addstr(msg, it->c_str());
   }

   // Create the socket and send the register command
   zsock_t * client = zsock_new_req ("ipc://riapsdiscoveryservice");
   assert(client);

   // TODO check return value
   zmsg_send(&msg, client);

   // Wait for the OK response
   // TODO: Specify OK response, error handling
   auto result = zstr_recv(client);
   std::cout << result;
   // TODO: if (result == somethingsomething) {}

   // release ZMQ socket
   zsock_destroy(&client);
}

bool deregister_service(std::string service_name) {
    zmsg_t* msg = zmsg_new();

    std::cout << "Deregistering service " << service_name;

    zmsg_addstr(msg, CMD_DISC_DEREGISTER_SERVICE);
    zmsg_addstr(msg, service_name.c_str());

    zsock_t * client = zsock_new_req ("ipc://riapsdiscoveryservice");
    assert(client);

    // TODO check return value
    zmsg_send(&msg, client);

    char* msg_response = zstr_recv(client);

    if (!msg_response){
        std::cout << "No msg => interrupted" << std::endl;
        return false;
    }
    else{
        free(msg_response);
    }

    zsock_destroy(&client);
    return true;
}

bool
get_servicenames(std::vector<std::string>& service_list) {
   zmsg_t* msg = zmsg_new();
   zmsg_addstr(msg, "$GETSERVICES$");
   zsock_t * client = zsock_new_req ("ipc://riapsdiscoveryservice");
   assert(client);

   // TODO check return value
   zmsg_send(&msg, client);

   zmsg_t* msg_response = zmsg_recv(client);

   if (!msg_response){
       std::cout << "No msg => interrupted" << std::endl;
       return false;
   }

   char* command = zmsg_popstr(msg_response);

   if (streq(command, "$SERVICES$")) {
       bool has_more_msg = true;
       while (has_more_msg){
            char* param = zmsg_popstr(msg_response);
            if (param){
                service_list.push_back(param);
                free(param);
            } else{
                has_more_msg = false;
            }
        }
   }



   // Wait for the OK response
   // TODO: Specify OK response, error handling
   //auto result = zstr_recv(client);
   //std::cout << result;

   zsock_destroy(&client);

   return false; 
}

bool
get_servicebyname(std::string service_name, std::vector<service_details>& services){

    zmsg_t* msg = zmsg_new();
    zmsg_addstr(msg, CMD_DISC_GETSERVICE_BY_NAME);
    zmsg_addstr(msg, service_name.c_str());
    zsock_t * client = zsock_new_req ("ipc://riapsdiscoveryservice");
    assert(client);

    // TODO check return value
    zmsg_send(&msg, client);

    zmsg_t* msg_response = zmsg_recv(client);

    if (!msg_response){
       std::cout << "No msg => interrupted" << std::endl;
       return false;
    }

    std::vector<zmsg_t*> responseframes;
    extract_zmsg(msg_response, responseframes);

    for (auto it = responseframes.begin(); it!=responseframes.end(); it++) {
        std::vector<std::string> params;
        service_details current_service;

        extract_params(*it, params);

        params_to_service_details(params, current_service);

        services.push_back(current_service);



        zmsg_destroy(&(*it));
    }

    zmsg_destroy(&msg_response);




   //bool has_more_msg = true;
   //while (has_more_msg){
//       zmsg_t* current_service = zmsg_popmsg(msg_response);
//       if (current_service){
//
//
//        free(current_service);
//    } else{
//        has_more_msg = false;
//    }
//    }
   
//   char* command = zmsg_popstr(msg_response);
//
//   if (streq(command, "$SERVICES$")) {
//       bool has_more_msg = true;
//       while (has_more_msg){
//            char* param = zmsg_popstr(msg_response);
//            if (param){
//                service_list.push_back(param);
//                free(param);
//            } else{
//                has_more_msg = false;
//            }
//        }
//   }



   // Wait for the OK response
   // TODO: Specify OK response, error handling
   //auto result = zstr_recv(client);
   //std::cout << result;

   zsock_destroy(&client);

   return false; 
    
}

bool
get_servicebyname_async(std::string service_name, std::string replyaddress){

    zmsg_t* msg = zmsg_new();
    zmsg_addstr(msg, CMD_DISC_GETSERVICE_BY_NAME_ASYNC);
    zmsg_addstr(msg, service_name.c_str());
    zmsg_addstr(msg, replyaddress.c_str());
    zsock_t * client = zsock_new_req ("ipc://riapsdiscoveryservice");
    assert(client);

    // TODO check return value
    zmsg_send(&msg, client);

    zmsg_t* msg_response = zmsg_recv(client);

    if (!msg_response){
        std::cout << "No msg => interrupted" << std::endl;
        return false;
    }


    zmsg_destroy(&msg_response);

    zsock_destroy(&client);

    return false;

}

void
ping_service(std::string service_name){
    zmsg_t* msg = zmsg_new();
    zmsg_addstr(msg, CMD_DISC_PING);
    zmsg_addstr(msg, service_name.c_str());
    zsock_t * client = zsock_new_req ("ipc://riapsdiscoveryservice");
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
}

void
register_actor(std::string actorname){
    zmsg_t* msg = zmsg_new();
    zmsg_addstr(msg, CMD_DISC_REGISTER_ACTOR);
    zmsg_addstr(msg, actorname.c_str());
    zsock_t * client = zsock_new_req ("ipc://riapsdiscoveryservice");
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

    zsock_destroy(&client);
}

void
deregister_actor(std::string actorname){
    zmsg_t* msg = zmsg_new();
    zmsg_addstr(msg, CMD_DISC_DEREGISTER_ACTOR);
    zmsg_addstr(msg, actorname.c_str());
    zsock_t * client = zsock_new_req ("ipc://riapsdiscoveryservice");
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

    zsock_destroy(&client);
}

void
register_component(std::string actorname, std::string componentname) {
    // Message format:
    // 1) actorname
    // 2) componentname

    std::cout << "Registering " + actorname + " " + componentname << std::endl;

    zmsg_t* msg = zmsg_new();
    zmsg_addstr(msg, CMD_DISC_REGISTER_COMPONENT);
    zmsg_addstr(msg, actorname.c_str());
    zmsg_addstr(msg, componentname.c_str());
    zsock_t * client = zsock_new_req ("ipc://riapsdiscoveryservice");
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

    zsock_destroy(&client);
}

void
deregister_component(std::string actorname, std::string componentname) {
    zmsg_t* msg = zmsg_new();
    zmsg_addstr(msg, CMD_DISC_DEREGISTER_COMPONENT);
    zmsg_addstr(msg, actorname.c_str());
    zmsg_addstr(msg, componentname.c_str());
    zsock_t * client = zsock_new_req ("ipc://riapsdiscoveryservice");
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

    zsock_destroy(&client);
}
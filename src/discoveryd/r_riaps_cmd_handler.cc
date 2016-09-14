//
// Created by parallels on 9/14/16.
//

#include <discoveryd/r_riaps_cmd_handler.h>

void init_command_mappings() {
    handler_mapping[CMD_DISC_REGISTER_SERVICE] = &handle_register_service;
}

bool handle_command(std::string command, zmsg_t* msg, zsock_t* inbox_socket) {

    if (handler_mapping.find(command) == handler_mapping.end()){
        return false;
    }

    handler_mapping[command](msg, inbox_socket);
}

void handle_register_service(zmsg_t* msg, zsock_t* inbox_socket) {
    std::vector<std::string> message_parameters;

    service_details service;
    extract_params(msg, message_parameters);
    params_to_service_details(message_parameters, service);

    registerService(service);
    zstr_send(inbox_socket, "REGISTERED");
}
//
// Created by parallels on 9/13/16.
//

#ifndef RIAPS_FW_R_RIAPS_CMD_HANDLER_H
#define RIAPS_FW_R_RIAPS_CMD_HANDLER_H

#include "componentmodel/r_riaps_actor_commands.h"
#include "utils/r_message.h"
#include "discoveryd/r_consul.h"

#include <map>
#include <string>
#include <functional>

#include <czmq.h>

static std::map<std::string, std::function<void(zmsg_t*, zsock_t*, zactor_t*)>> handler_mapping;

void init_command_mappings();

bool handle_command(std::string command, zmsg_t* msg, zsock_t* replysocket, zactor_t* asyncactor);
void handle_register_service(zmsg_t* msg, zsock_t* replysocket, zactor_t* asyncactor);
void handle_deregister_service(zmsg_t* msg, zsock_t* replysocket, zactor_t* asyncactor);
void handle_getservices(zmsg_t* msg, zsock_t* replysocket, zactor_t* asyncactor);
void handle_getservicebyname(zmsg_t* msg, zsock_t* replysocket, zactor_t* asyncactor);
void handle_getservicebyname_async(zmsg_t* msg, zsock_t* replysocket, zactor_t* asyncactor);
void handle_registernode(zmsg_t* msg, zsock_t* replysocket, zactor_t* asyncactor);

#endif //RIAPS_FW_R_RIAPS_CMD_HANDLER_H

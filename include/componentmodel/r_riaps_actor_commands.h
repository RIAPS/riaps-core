//
// Created by parallels on 9/8/16.
//

#ifndef RIAPS_FW_R_RIAPS_ACTOR_COMMANDS_H
#define RIAPS_FW_R_RIAPS_ACTOR_COMMANDS_H

#include <vector>
#include <string>

#define CMD_DISC_REGISTER_NODE                   "$REGISTERNODE$" // Registers a node (the hardware) in the key-value storage of disc. srv.
#define CMD_DISC_DEREGISTER_NODE                 "$DEREGISTERNODE$"

#define CMD_DISC_REGISTER_ACTOR                  "$REGISTERACTOR$"
#define CMD_DISC_DEREGISTER_ACTOR                "$DEREGISTERACTOR$"

#define CMD_DISC_DEREGISTER_COMPONENT            "$DEREGISTERCOMPONENT$"
#define CMD_DISC_REGISTER_COMPONENT              "$REGISTERCOMPONENT$"

#define CMD_DISC_REGISTER_SERVICE                "$REGISTER$"
#define CMD_DISC_DEREGISTER_SERVICE              "$DEREGISTER$"

#define CMD_DISC_GETSERVICE_BY_NAME              "$GETSERVICEBYNAME$"
#define CMD_DISC_GETSERVICE_BY_NAME_ASYNC        "$GETSERVICEBYNAMEASYNC$"
#define CMD_DISC_GETSERVICE_BY_NAME_POLL_ASYNC   "$GETSERVICEBYNAMEPOLLASYNC$"
#define CMD_DISC_GET_SERVICES                    "$GETSERVICES"
#define CMD_DISC_PING                            "$PING$"

#define MSG_SERVICE_LIST_HEADER                  "$SERVICES$"

//#define CMD_DISC_ALL (std::vector<std::string>({CMD_DISC_REGISTER_NODE}))


#endif //RIAPS_FW_R_RIAPS_ACTOR_COMMANDS_H

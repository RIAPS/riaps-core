//
// Created by parallels on 9/13/16.
//

#ifndef RIAPS_FW_R_RIAPS_CMD_HANDLER_H
#define RIAPS_FW_R_RIAPS_CMD_HANDLER_H

#include "componentmodel/r_riaps_actor_commands.h"
#include "utils/r_message.h"
#include "messaging/disco.capnp.h"

#include <map>
#include <string>
#include <functional>

#include <czmq.h>

static std::map<Kind, std::string> kindMap = {{Kind::PUB, "pub"},
                                              {Kind::SUB, "sub"},
                                              {Kind::CLT, "clt"},
                                              {Kind::SRV, "srv"},
                                              {Kind::REQ, "req"},
                                              {Kind::REP, "rep"}};

std::pair<std::string, std::string>
buildInsertKeyValuePair(std::string appName ,
                        std::string msgType ,
                        Kind        kind    ,
                        Scope       scope   ,
                        std::string host    ,
                        uint16_t    port    );

std::pair<std::string, std::string>
buildLookupKey(std::string appName            ,
               std::string msgType            ,
               Kind        kind               ,
               Scope       scope              ,
               std::string clientActorHost    ,
               std::string clientActorName    ,
               std::string clientInstanceName ,
               std::string clientPortName     );

#endif //RIAPS_FW_R_RIAPS_CMD_HANDLER_H

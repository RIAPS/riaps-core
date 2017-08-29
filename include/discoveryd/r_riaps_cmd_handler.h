//
// Created by parallels on 9/13/16.
//

#ifndef RIAPS_FW_R_RIAPS_CMD_HANDLER_H
#define RIAPS_FW_R_RIAPS_CMD_HANDLER_H

#include "componentmodel/r_riaps_actor_commands.h"
#include "utils/r_message.h"
#include "messaging/disco.capnp.h"
#include "discoveryd/r_discovery_types.h"

#include <opendht.h>

#include <map>
#include <string>
#include <functional>

#include <czmq.h>

#define DHT_ROUTER_CHANNEL "ipc:///tmp/dhtrouterchannel"

static std::map<riaps::discovery::Kind, std::string> kindMap =
                                             {{riaps::discovery::Kind::PUB, "pub"},
                                              {riaps::discovery::Kind::SUB, "sub"},
                                              {riaps::discovery::Kind::CLT, "clt"},
                                              {riaps::discovery::Kind::SRV, "srv"},
                                              {riaps::discovery::Kind::REQ, "req"},
                                              {riaps::discovery::Kind::REP, "rep"}};

std::pair<std::string, std::string>
buildInsertKeyValuePair(std::string appName ,
                        std::string msgType ,
                        riaps::discovery::Kind        kind    ,
                        riaps::discovery::Scope       scope   ,
                        std::string host    ,
                        uint16_t    port    );

std::pair<std::string, std::string>
buildLookupKey(std::string appName            ,
               std::string msgType            ,
               riaps::discovery::Kind        kind               ,
               riaps::discovery::Scope       scope              ,
               std::string clientActorHost    ,
               std::string clientActorName    ,
               std::string clientInstanceName ,
               std::string clientPortName     );


// Message handlers

bool handlePipeMessage(zsock_t*        pipeSocket,
                       dht::DhtRunner& dhtNode);

bool handleRiapsMessages(zsock_t*                                                               riapsSocket,
                         std::map<std::string, std::unique_ptr<actor_details_t>>&               clients,
                         std::map<pid_t, std::vector<std::unique_ptr<service_checkins_t>>>&     serviceCheckins,
                         std::map<std::string, std::vector<std::unique_ptr<client_details_t>>>& clientSubscriptions,
                         std::map<std::string, std::future<size_t>>&                            registeredListeners,
                         const std::string&                                                     hostAddress,
                         const std::string&                                                     macAddress,
                         const std::map<std::string, int64_t>&                                  zombieList,
                         dht::DhtRunner&                                                        dhtNode
);

#endif //RIAPS_FW_R_RIAPS_CMD_HANDLER_H

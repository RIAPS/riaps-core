//
// Created by istvan on 11/22/16.
//

#ifndef RIAPS_FW_R_ODHT_H
#define RIAPS_FW_R_ODHT_H

#include <string>
#include <opendht.h>
#include <messaging/disco.capnp.h>
#include <discoveryd/r_riaps_actor.h>
#include <map>
#include <string>
#include <memory>
#include <vector>

struct _client_details;
struct _actor_details;

typedef struct _client_details client_details;
typedef struct _actor_details actor_details;

#define RIAPS_DHT_NODE_PORT 4222

void dhtJoinToCluster(std::string&, int port, dht::DhtRunner&);

void handleGet(const ProviderListGet::Reader& msgProviderGet,
               const std::map<std::string, std::unique_ptr<actor_details>>& clients);

void handleUpdate(const ProviderListUpdate::Reader&                                          msgProviderUpdate,
                  const std::map<std::string, std::vector<std::unique_ptr<client_details>>>& clientSubscriptions,
                  const std::map<std::string, std::unique_ptr<actor_details>>&               clients);

#endif //RIAPS_FW_R_ODHT_H

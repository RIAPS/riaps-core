//
// Created by istvan on 11/22/16.
//

#ifndef RIAPS_FW_R_ODHT_H
#define RIAPS_FW_R_ODHT_H

#include <messaging/disco.capnp.h>
#include <discoveryd/r_riaps_actor.h>
#include <discoveryd/r_discovery_types.h>

#include <opendht.h>


#include <string>
#include <map>
#include <string>
#include <memory>
#include <vector>



#define RIAPS_DHT_NODE_PORT 4222

void dhtJoinToCluster(std::string&, int port, dht::DhtRunner&);

void handleGet(const riaps::discovery::ProviderListGet::Reader& msgProviderGet,
               const std::map<std::string, std::unique_ptr<actor_details_t>>& clients);

void handleUpdate(const riaps::discovery::ProviderListUpdate::Reader&                          msgProviderUpdate,
                  const std::map<std::string, std::vector<std::unique_ptr<client_details_t>>>& clientSubscriptions,
                  const std::map<std::string, std::unique_ptr<actor_details_t>>&               clients);

#endif //RIAPS_FW_R_ODHT_H

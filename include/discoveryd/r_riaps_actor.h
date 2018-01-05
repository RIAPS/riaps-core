#ifndef R_RIAPS_ACTOR
#define R_RIAPS_ACTOR

#include <discoveryd/r_discovery_types.h>
#include <componentmodel/r_discoverdapi.h>

#include <czmq.h>

void riaps_actor (zsock_t *pipe, void *args);
int deregisterActor(const std::string& appName,
                    const std::string& actorName,
                    const std::string& macAddress,
                    const std::string& hostAddress,
                    std::map<std::string, std::unique_ptr<actor_details_t>>& clients);

//void maintainRenewal(std::map<pid_t, std::vector<std::unique_ptr<service_checkins_t>>>& serviceCheckins, dht::DhtRunner& dhtNode);



//void maintainZombieList(std::map<std::string, int64_t>& zombieList);



#endif
#ifndef R_RIAPS_ACTOR
#define R_RIAPS_ACTOR

#include <discoveryd/r_discovery_types.h>
#include <componentmodel/r_discoverdapi.h>

#include <czmq.h>

namespace riaps::discovery {
    void riaps_actor(zsock_t *pipe, void *args);

    int deregisterActor(const std::string &appName,
                        const std::string &actorName,
                        const std::string &macAddress,
                        const std::string &hostAddress,
                        std::map<std::string, std::unique_ptr<ActorDetails>> &clients);
}

#endif
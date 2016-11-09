#ifndef R_DISCOVERD_API
#define R_DISCOVERD_API

#include <string>
#include <vector>
#include <czmq.h>
#include <iostream>

#include <capnp/message.h>
#include <capnp/serialize.h>

#include "json.h"
#include "utils/r_message.h"
#include "r_riaps_actor_commands.h"
#include "messaging/disco.capnp.h"






extern bool
get_servicenames(std::vector<std::string>&);

extern bool
get_servicebyname(std::string service_id, std::vector<service_details>& services);

extern bool
get_servicebyname_async(std::string service_name, std::string replyaddress);

extern bool
get_servicebyname_poll_async(std::string service_name, std::string replyaddress);

extern bool
deregister_service(std::string service_name);

extern void
ping_service(std::string service_name);

extern void
register_component(std::string actorname, std::string componentname);

extern void
deregister_component(std::string actorname, std::string componentname);

extern zsock_t*
register_actor(std::string appname, std::string actorname);

extern void
deregister_actor(std::string actorname);

extern bool
register_service(std::string              service_id   ,
                 std::string              service_name ,
                 std::string              ip_address   ,
                 std::string              port         ,
                 std::vector<std::string> tags
);


#endif
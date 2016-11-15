#ifndef R_DISCOVERD_API
#define R_DISCOVERD_API

#define DISCOVERY_SERVICE_IPC "ipc:///tmp/riapsdiscoveryservice"

#include <string>
#include <vector>
#include <czmq.h>
#include <iostream>

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <capnp/serialize-packed.h>

#include "json.h"
#include "utils/r_message.h"
#include "r_riaps_actor_commands.h"
#include "messaging/disco.capnp.h"
#include "componentmodel/r_componentbase.h"



struct _service_lookup_result {
    std::string part_name; // instance name
    std::string port_name;
    std::string host_name;
    uint16_t    port;

};

typedef struct _service_lookup_result service_lookup_result;


extern bool
get_servicenames(std::vector<std::string>&);

extern bool
get_servicebyname(std::string service_id, std::vector<service_details>& services);

extern bool
get_servicebyname_async(std::string service_name, std::string replyaddress);

extern std::vector<service_lookup_result>
subscribe_to_service(std::string app_name  ,
                     std::string part_name , // instance_name
        //                       std::string part_type ,
                     Kind        kind      ,
                     Scope       scope     ,
                     std::string port_name ,
                     std::string msg_type  );// PortType


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
register_service(std::string              app_name     ,
                 std::string              message_type ,
                 std::string              ip_address   ,
                 uint16_t                 port         ,
                 Kind                     kind         ,
                 Scope                    scope        ,
                 std::vector<std::string> tags
);




#endif
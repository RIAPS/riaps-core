#ifndef R_DISCOVERD_API
#define R_DISCOVERD_API

//#define DISCOVERY_SERVICE_IPC "ipc:///tmp/riapsdiscoveryservice"

#define DISCOVERY_SERVICE_IPC "tcp://127.0.0.1:75846"


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

extern std::vector<service_lookup_result>
subscribe_to_service(std::string app_name  ,
                     std::string part_name , // instance_name
        //                       std::string part_type ,
                     Kind        kind      ,
                     Scope       scope     ,
                     std::string port_name ,
                     std::string msg_type  );// PortType
extern bool
register_service(std::string              app_name     ,
                 std::string              message_type ,
                 std::string              ip_address   ,
                 uint16_t                 port         ,
                 Kind                     kind         ,
                 Scope                    scope        ,
                 std::vector<std::string> tags
);


extern zsock_t*
register_actor(std::string appname, std::string actorname);

#endif
#ifndef R_DISCOVERD_API
#define R_DISCOVERD_API


#include <string>
#include <vector>
#include <czmq.h>
#include <iostream>

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <capnp/serialize-packed.h>

#include <json.h>
#include <utils/r_message.h>
#include "r_riaps_actor_commands.h"
#include <messaging/disco.capnp.h>
#include <componentmodel/r_componentbase.h>
#include <common/r_group.h>




struct _service_lookup_result {
    std::string part_name; // instance name
    std::string port_name;
    std::string host_name;
    uint16_t    port;
};

typedef struct _service_lookup_result service_lookup_result;

extern std::vector<service_lookup_result>
subscribeToService(const std::string&      app_name   ,
                     const std::string&      part_name  , // instance_name
                     const std::string&      actor_name ,
                     riaps::discovery::Kind  kind       ,
                     riaps::discovery::Scope scope      ,
                     const std::string&      port_name  ,
                     const std::string&      msg_type     // PortType
                     );
extern bool
registerService(const std::string&              app_name     ,
                const std::string&              message_type ,
                const std::string&              ip_address   ,
                const uint16_t&                 port         ,
                riaps::discovery::Kind          kind         ,
                riaps::discovery::Scope         scope        ,
                const std::vector<std::string>& tags
);

extern bool
joinGroup(const std::string& appName,
          const riaps::groups::GroupId& groupId,
          const std::vector<riaps::groups::GroupService>& groupServices
);


extern zsock_t*
registerActor(const std::string& appname, const std::string& actorname);

extern void
deregisterActor(const std::string& actorName, const std::string& appName);

#endif
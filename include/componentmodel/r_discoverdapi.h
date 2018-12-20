#ifndef R_DISCOVERD_API
#define R_DISCOVERD_API

#include <componentmodel/r_riaps_actor_commands.h>
#include <componentmodel/r_componentbase.h>
#include <groups/r_group.h>
#include <messaging/disco.capnp.h>

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <capnp/serialize-packed.h>
#include <json.h>

#include <string>
#include <vector>
#include <czmq.h>
#include <iostream>

namespace riaps {

    namespace groups{
        struct GroupId;
        struct GroupService;
    }

    namespace discovery {

        struct ServiceLookupResult {
            std::string part_name; // instance name
            std::string port_name;
            std::string host_name;
            uint16_t    port;
        };

        class Disco {
        public :
            static std::vector<ServiceLookupResult> SubscribeToService(const std::string &app_name,
                                                                       const std::string &part_name, // instance_name
                                                                       const std::string &actor_name,
                                                                       const std::string &ip_address,
                                                                       riaps::discovery::Kind kind,
                                                                       riaps::discovery::Scope scope,
                                                                       const std::string &port_name,
                                                                       const std::string &msg_type     // port_type
            );

            static bool RegisterService(const std::string             &app_name     ,
                                        const std::string              &actor_name  ,
                                        const std::string              &message_type,
                                        const std::string              &ip_address  ,
                                        const uint16_t                 &port        ,
                                        riaps::discovery::Kind         kind         ,
                                        riaps::discovery::Scope        scope        ,
                                        const std::vector<std::string> &tags
            );

            static bool JoinGroup(const std::string &app_name,
                                  const std::string &component_id,
                                  const riaps::groups::GroupId &group_id,
                                  const std::vector<riaps::groups::GroupService> &group_services
            );

            static zsock_t* RegisterActor(const std::string& app_name, const std::string& actor_name);

            static void DeregisterActor(const std::string& actor_name, const std::string& app_name);
        };
    }
}





#endif
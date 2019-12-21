//
// Created by istvan on 12/5/19.
//

#ifndef RIAPS_CORE_R_GROUPSERVICE_H
#define RIAPS_CORE_R_GROUPSERVICE_H

#include <groups/r_groupid.h>

namespace riaps {
    namespace groups {
        /**
         * Description of one port in the group.
         */
        struct GroupService {
            std::string message_type;
            // std::string address;
            std::string host;
            int port;
            MSGPACK_DEFINE(message_type, host, port);
        };

        /**
         * All the details of the group in one structure.
         */
        struct GroupDetails {
            std::string               app_name;
            std::string               component_id;
            GroupId                   group_id;
            std::vector<GroupService> group_services;
            MSGPACK_DEFINE(app_name, component_id, group_id, group_services);
        };
    }
}



#endif //RIAPS_CORE_R_GROUPSERVICE_H

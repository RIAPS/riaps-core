//
// Created by istvan on 9/27/17.
//

#ifndef RIAPS_CORE_R_GROUP_H_H
#define RIAPS_CORE_R_GROUP_H_H

#include <msgpack.hpp>

#include <string>
#include <vector>

namespace riaps {
    namespace groups {

        struct GroupId {
            std::string groupName;
            std::string groupType;

            MSGPACK_DEFINE(groupName, groupType);
        };

        struct GroupService {
            std::string messageType;
            std::string address;
            MSGPACK_DEFINE(messageType, address);
        };

        struct GroupDetails {
            std::string               appName;
            GroupId                   groupId;
            std::vector<GroupService> groupServices;
            MSGPACK_DEFINE(appName, groupId, groupServices);

        };

    }
}
#endif //RIAPS_CORE_R_GROUP_H_H

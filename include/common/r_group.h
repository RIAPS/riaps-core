//
// Created by istvan on 9/27/17.
//

#ifndef RIAPS_CORE_R_GROUP_H_H
#define RIAPS_CORE_R_GROUP_H_H

namespace riaps {
    namespace groups {

        struct GroupId {
            std::string groupName;
            std::string groupType;
        };

        struct GroupService {
            std::string messageType;
            std::string address;
        };

        struct GroupDetails {
            std::string               appName;
            GroupId                   groupId;
            std::vector<GroupService> groupServices;
        };
    }
}
#endif //RIAPS_CORE_R_GROUP_H_H

//
// Created by istvan on 9/21/17.
//

#ifndef RIAPS_CORE_R_GROUPBASE_H_H
#define RIAPS_CORE_R_GROUPBASE_H_H

#include "r_publisherport.h"
#include "r_subscriberport.h"

#include <czmq.h>

#include <vector>
#include <map>
#include <string>

namespace riaps {

    namespace ports {
        class PublisherPort;
        class SubscriberPort;
    }

    namespace groups {

        struct GroupId{
            std::string groupName;
            std::string groupType;
        };

        class Groups {
        public:
            Groups();
            void JoinGroup(const GroupId& groupId, const std::string& appName);
        private:
            void InitPorts();
            void InitControlPort();
            std::map<GroupId, zsock_t*> _ports;
        };
    }
}
#endif //RIAPS_CORE_R_GROUPBASE_H_H

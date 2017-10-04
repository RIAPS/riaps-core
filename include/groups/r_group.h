//
// Created by istvan on 9/27/17.
//

#ifndef RIAPS_CORE_R_GROUP_H_H
#define RIAPS_CORE_R_GROUP_H_H

#include <componentmodel/r_configuration.h>
#include <componentmodel/r_publisherport.h>
#include <componentmodel/r_subscriberport.h>

#include <msgpack.hpp>
#include <czmq.h>

#include <string>
#include <vector>
#include <map>

namespace riaps {
    namespace groups {

        struct GroupId {
            //GroupId();
            //~GroupId();

            std::string groupName;
            std::string groupTypeId;

            MSGPACK_DEFINE(groupName, groupTypeId);
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

        /**
         * Encapsulates a RIAPS Group.
         *  - Creates/releases/stores communication ports
         *  - Registers group in the discovery service
         *  - Receives messages
         *  - Sends messages
         */
        class Group {
        public:
            Group(const GroupId& groupId);
            void InitGroup(const groupt_conf& groupTypeConf);

            const groupt_conf& GetConfig() const;
            virtual ~Group();

        protected:
            const GroupId     _groupId;
            const groupt_conf _groupTypeConf;

            std::unique_ptr<riaps::ports::PublisherPort>  _groupPubPort;
            std::unique_ptr<riaps::ports::SubscriberPort> _groupSubPort;

            std::map<std::string, riaps::ports::PortBase*> _groupPorts;
        };

    }
}
#endif //RIAPS_CORE_R_GROUP_H_H

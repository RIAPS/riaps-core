//
// Created by istvan on 9/27/17.
//

#ifndef RIAPS_CORE_R_GROUP_H_H
#define RIAPS_CORE_R_GROUP_H_H

#include <componentmodel/r_configuration.h>
#include <componentmodel/r_pubportgroup.h>
#include <componentmodel/r_subscriberport.h>

#include <msgpack.hpp>
#include <czmq.h>

#include <string>
#include <vector>
#include <map>

#define INTERNAL_PUB_NAME "$PUB#"
#define INTERNAL_MESSAGETYPE "InternalGroupMessage"

namespace riaps {
    namespace groups {

        /**
         * The instance name of the group and the group type id (form the config) are the GroupId
         */
        struct GroupId {
            std::string groupName;
            std::string groupTypeId;

            MSGPACK_DEFINE(groupName, groupTypeId);
        };

        /**
         * Description of one port in the group.
         */
        struct GroupService {
            std::string messageType;
            std::string address;
            MSGPACK_DEFINE(messageType, address);
        };

        /**
         * All the details of the group in one structure.
         */
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

            /**
             * Initializes a group, by the given groupId
             * @param groupId Must have valid configuration entry with the matching id.
             */
            Group(const GroupId& groupId);

            /**
             * Creates the communication ports and registers the group in the discovery service.
             * @return
             */
            bool InitGroup();

            virtual ~Group();

        protected:
            const GroupId _groupId;
            groupt_conf   _groupTypeConf;

            /**
             * Always store the communication ports in unique_ptr (self-defense)
             */
            std::unique_ptr<riaps::ports::GroupPublisherPort>    _groupPubPort;
            std::unique_ptr<riaps::ports::SubscriberPort>        _groupSubPort;

            std::vector<std::unique_ptr<riaps::ports::PortBase>> _groupPorts;
        };

    }
}
#endif //RIAPS_CORE_R_GROUP_H_H

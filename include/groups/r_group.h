//
// Created by istvan on 9/27/17.
//

#ifndef RIAPS_CORE_R_GROUP_H_H
#define RIAPS_CORE_R_GROUP_H_H

#include <componentmodel/r_actor.h>
#include <componentmodel/r_configuration.h>
#include <componentmodel/r_pubportgroup.h>
#include <componentmodel/r_subportgroup.h>
#include <messaging/disco.capnp.h>

#include <msgpack.hpp>
#include <czmq.h>

#include <string>
#include <vector>
#include <map>

#define INTERNAL_SUB_NAME "$SUB#"
#define INTERNAL_PUB_NAME "$PUB#"
#define INTERNAL_MESSAGETYPE "InternalGroupMessage"

namespace riaps {
    namespace groups {

        /**
         * The instance name of the group and the group type id (form the config) are the GroupId
         */
        struct GroupId {
            std::string groupTypeId;
            std::string groupName;


            /**
             * To use GroupId in std::map as key
             * @param other
             * @return
             */
            bool operator<(const GroupId& other) const;

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
            std::string               componentId;
            GroupId                   groupId;
            std::vector<GroupService> groupServices;
            MSGPACK_DEFINE(appName, componentId, groupId, groupServices);

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
            Group(const GroupId& groupId, const std::string& componentId);

            /**
             * Creates the communication ports and registers the group in the discovery service.
             * @return true if the ports were succesfully created and registered False otherwise.
             */
            bool InitGroup();

            void ConnectToNewServices(riaps::discovery::GroupUpdate::Reader& msgGroupUpdate);

            virtual ~Group();

        protected:
            const GroupId     _groupId;
            groupt_conf       _groupTypeConf;
            const std::string _componentId;

            /**
             * Always store the communication ports in unique_ptr (self-defense)
             */
            std::unique_ptr<riaps::ports::GroupPublisherPort>    _groupPubPort;
            std::unique_ptr<riaps::ports::GroupSubscriberPort>   _groupSubPort;

            std::vector<std::unique_ptr<riaps::ports::PortBase>> _groupPorts;
        };

    }
}
#endif //RIAPS_CORE_R_GROUP_H_H

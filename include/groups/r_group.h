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
#include <messaging/distcoord.capnp.h>

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
        class Group final {
        public:

            /**
             * Initializes a group, by the given groupId
             * @param groupId Must have valid configuration entry with the matching id.
             */
            Group(const GroupId& groupId, const std::string _componentId);

            /**
             * Creates the communication ports and registers the group in the discovery service.
             * @return true if the ports were succesfully created and registered False otherwise.
             */
            bool InitGroup();

            void ConnectToNewServices(riaps::discovery::GroupUpdate::Reader& msgGroupUpdate);

            bool SendMessage(capnp::MallocMessageBuilder& message, const std::string& portName);

            ports::GroupSubscriberPort* FetchNextMessage(std::unique_ptr<capnp::FlatArrayMessageReader>& messageReader);

            void SendHeartBeat(riaps::distrcoord::HeartBeatType type);
            
            std::shared_ptr<std::vector<std::string>> GetKnownComponents();

            uint16_t GetMemberCount(uint16_t timeout /*msec*/) const;

            ~Group();

        private:
            const GroupId     _groupId;
            groupt_conf       _groupTypeConf;

            /**
             * Always store the communication ports in unique_ptr (self-defense)
             */
            std::shared_ptr<riaps::ports::GroupPublisherPort>    _groupPubPort;
            std::shared_ptr<riaps::ports::GroupSubscriberPort>   _groupSubPort;

            std::map<const zsock_t*, std::shared_ptr<riaps::ports::PortBase>> _groupPorts;
            std::unordered_map<std::string, int64_t> _knownNodes;

            zframe_t*  _lastFrame;
            zpoller_t* _groupPoller;

            std::string _componentId;

            int64_t _lastPing;
            const uint16_t _pingPeriod;
        };

    }
}
#endif //RIAPS_CORE_R_GROUP_H_H

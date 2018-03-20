//
// Created by istvan on 9/27/17.
//

#ifndef RIAPS_CORE_R_GROUP_H_H
#define RIAPS_CORE_R_GROUP_H_H

#include <componentmodel/r_actor.h>
#include <componentmodel/r_configuration.h>
#include <groups/r_grouplead.h>
#include <componentmodel/ports/r_pubportgroup.h>
#include <componentmodel/ports/r_subportgroup.h>
#include <messaging/disco.capnp.h>
#include <messaging/distcoord.capnp.h>
#include <utils/r_timeout.h>

#include <spdlog/spdlog.h>
#include <msgpack.hpp>
#include <czmq.h>

#include <string>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include <set>

#define INTERNAL_SUB_NAME "$SUB#"
#define INTERNAL_PUB_NAME "$PUB#"
#define INTERNAL_MESSAGETYPE "InternalGroupMessage"

using namespace std::chrono;
using namespace riaps::utils;

namespace spd = spdlog;

namespace riaps {
    namespace groups {

        class GroupLead;

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

            bool operator==(const GroupId& other) const;


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
            Group(const GroupId& groupId, ComponentBase* parentComponent);

            /**
             * Creates the communication ports and registers the group in the discovery service.
             * @return true if the ports were succesfully created and registered False otherwise.
             */
            bool InitGroup();

            void ConnectToNewServices(riaps::discovery::GroupUpdate::Reader& msgGroupUpdate);

            bool SendMessage(capnp::MallocMessageBuilder& message, const std::string& portName);
            bool SendMessage(zmsg_t** message, const std::string& portName);

            bool SendInternalMessage(capnp::MallocMessageBuilder& message);

            ports::GroupSubscriberPort* FetchNextMessage(std::shared_ptr<capnp::FlatArrayMessageReader>& messageReader);

            bool SendPingWithPeriod();
            bool SendPing();
            bool SendPong();

            bool SendMessageToLeader(capnp::MallocMessageBuilder& message);

            bool ProposeValueToLeader(capnp::MallocMessageBuilder &message, const std::string &proposeId);
            bool SendVote(const std::string& proposeId, bool accept);

            bool ProposeActionToLeader(const std::string& proposeId,
                                       const std::string &actionId,
                                       const timespec &absTime);

            const ComponentBase* GetParentComponent() const;
            const std::string GetParentComponentId() const;

            std::shared_ptr<std::set<std::string>> GetKnownComponents();

            /**
             * Counts the records in _knownNodes map
             * Before counting, the DeleteTimeoutNodes() is called.
             * @return Number of nodes in the group.
             */
            uint16_t GetMemberCount();

            std::string GetLeaderId();

            ~Group();
        private:
            /**
             * Delete records from the _knownNodes cache, it the Timer is exceeded
             * @return Number of deleted records.
             */
            uint32_t DeleteTimeoutNodes();
            bool SendHeartBeat(riaps::distrcoord::HeartBeatType type);

            GroupId     _groupId;
            groupt_conf       _groupTypeConf;

            /**
             * Always store the communication ports in unique_ptr (self-defense)
             */
            std::shared_ptr<riaps::ports::GroupPublisherPort>    _groupPubPort;
            std::shared_ptr<riaps::ports::GroupSubscriberPort>   _groupSubPort;

            std::map<const zsock_t*, std::shared_ptr<riaps::ports::PortBase>> _groupPorts;

            /**
             *
             *
             * List of ComponentID-s where the current group got PING/PONG messages from. Timestamped.
             *
             * @note The parent component is excluded from the list.
             *
             *  key   - component id (uuid, generated runtime, when the component starts
             *  value - timestamp of the last message from the given component
             */
            std::unordered_map<std::string, Timeout<std::milli>> _knownNodes;

            zframe_t*  _lastFrame;
            zpoller_t* _groupPoller;
            std::shared_ptr<capnp::FlatArrayMessageReader> _lastReader;

            std::shared_ptr<spd::logger> _logger;

            //int64_t  _lastPingSent;
            //float    _pingPeriod;

            Timeout<std::milli> _pingTimeout;


            std::random_device _rd;
            std::mt19937         _generator;
            std::uniform_int_distribution<int> _distrNodeTimeout;

            ComponentBase* _parentComponent;

            std::unique_ptr<riaps::groups::GroupLead> _groupLeader;

            // debug
            uint16_t _pingCounter;


        };

    }
}
#endif //RIAPS_CORE_R_GROUP_H_H

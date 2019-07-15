//
// Created by istvan on 9/27/17.
//

#ifndef RIAPS_CORE_R_GROUP_H_H
#define RIAPS_CORE_R_GROUP_H_H

#include <const/r_const.h>
#include <componentmodel/r_componentbase.h>
#include <componentmodel/r_configuration.h>
#include <componentmodel/ports/r_pubportgroup.h>
#include <componentmodel/ports/r_subportgroup.h>
#include <groups/r_grouplead.h>
#include <messaging/disco.capnp.h>
#include <messaging/distcoord.capnp.h>
#include <utils/r_timeout.h>

#include <spdlog_setup/conf.h>
#include <msgpack.hpp>
#include <czmq.h>

#include <string>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include <set>




namespace spd = spdlog;

namespace riaps {

    /**
     * RIAPS Distributed Coordination API
     */
    namespace groups {

        class GroupLead;

        /**
         * Uniquely identifies a group instance.
         */
        struct GroupId {
            /**
             * The group type, comes from the model file.
             */
            std::string group_type_id;

            /**
             * Name of the group, can be dynamic.
             */
            std::string group_name;

            bool operator<(const GroupId& other) const;
            bool operator==(const GroupId& other) const;

            ///\cond
            MSGPACK_DEFINE(group_name, group_type_id);
            ///\endcond
        };

        /**
         * Description of one port in the group.
         */
        struct GroupService {
            std::string message_type;
            std::string address;
            MSGPACK_DEFINE(message_type, address);
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
             * @param group_id Must have valid configuration entry with the matching id.
             */
            Group(const GroupId& group_id, ComponentBase* parentComponent);

            /**
             * Creates the communication ports and registers the group in the discovery service.
             * @return true if the ports were succesfully created and registered False otherwise.
             */
            bool InitGroup();

            void ConnectToNewServices(riaps::discovery::GroupUpdate::Reader& msgGroupUpdate);

            bool SendMessage(capnp::MallocMessageBuilder& message, const std::string& portName);
            bool SendMessage(zmsg_t** message, const std::string& portName);

            bool SendInternalMessage(capnp::MallocMessageBuilder& message);

            //ports::GroupSubscriberPort* FetchNextMessage(std::shared_ptr<capnp::FlatArrayMessageReader>& messageReader);
            void FetchNextMessage();

            bool SendPingWithPeriod();
            bool SendPing();
            bool SendPong();

            bool SendMessageToLeader(capnp::MallocMessageBuilder& message);
            bool SendLeaderMessage(capnp::MallocMessageBuilder& message);

            bool ProposeValueToLeader(capnp::MallocMessageBuilder &message, const std::string &proposeId);
            bool SendVote(const std::string& propose_id, bool accept);

            bool ProposeActionToLeader(const std::string& proposeId,
                                       const std::string &actionId,
                                       const timespec &absTime);

            const ComponentBase* parent_component() const;
            const std::string parent_component_id() const;

            std::shared_ptr<std::set<std::string>> GetKnownComponents();

            /**
             * Counts the records in _knownNodes map
             * Before counting, the DeleteTimeoutNodes() is called.
             * @return Number of nodes in the group.
             */
            uint16_t GetMemberCount();

            std::string leader_id() const;

            ~Group();
        private:
            /**
             * Delete records from the _knownNodes cache, it the Timer is exceeded
             * @return Number of deleted records.
             */
            uint32_t DeleteTimeoutNodes();
            bool SendHeartBeat(riaps::distrcoord::HeartBeatType type);

            GroupId     group_id_;
            GroupTypeConf group_type_conf_;

            /**
             * Always store the communication ports in shart_ptr
             */
            std::shared_ptr<riaps::ports::GroupPublisherPort>    group_pubport_;
            std::shared_ptr<riaps::ports::GroupSubscriberPort>   group_subport_;

            std::map<const zsock_t*, std::shared_ptr<riaps::ports::PortBase>> group_ports_;

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
            std::unordered_map<std::string, riaps::utils::Timeout<std::chrono::milliseconds>> known_nodes_;

            zpoller_t* group_poller_;

            std::shared_ptr<spd::logger> logger_;

            riaps::utils::Timeout<std::chrono::milliseconds> ping_timeout_;


            std::random_device random_device_;
            std::mt19937         random_generator_;
            std::uniform_int_distribution<int> timeout_distribution_;

            ComponentBase* parent_component_;

            std::unique_ptr<riaps::groups::GroupLead> group_leader_;
        };

    }
}

#endif //RIAPS_CORE_R_GROUP_H_H

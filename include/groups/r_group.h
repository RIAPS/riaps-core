//
// Created by istvan on 9/27/17.
//

#ifndef RIAPS_CORE_R_GROUP_H_H
#define RIAPS_CORE_R_GROUP_H_H

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <const/r_const.h>
#include <componentmodel/r_componentbase.h>
#include <componentmodel/r_configuration.h>
#include <componentmodel/ports/r_pubportgroup.h>
#include <componentmodel/ports/r_subportgroup.h>
#include <componentmodel/ports/r_queryport.h>
#include <componentmodel/r_messagebuilder.h>
#include <groups/r_groupid.h>
#include <groups/r_grouplead.h>
#include <groups/r_ownid.h>
#include <messaging/disco.capnp.h>
#include <messaging/dc.capnp.h>
#include <utils/r_timeout.h>
#include <utils/r_utils.h>

#include <spdlog_setup/conf.h>
#include <czmq.h>

#include <string>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include <set>
#include <unordered_map>

namespace spd = spdlog;

namespace riaps {

//    namespace ports {
//        class GroupPublisherPort;
//    }

    /**
     * RIAPS Distributed Coordination API
     */
    namespace groups {

        class GroupLead;

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
            bool HasLeader();

            /// OBSOLOTE
            //void ConnectToNewServices(riaps::discovery::GroupUpdate::Reader& msgGroupUpdate);

            void ConnectToNewServices(const std::string& address);

            //bool SendMessage(capnp::MallocMessageBuilder& message, const std::string& portName);
            bool SendMessage(zmsg_t** message);

            bool SendInternalMessage(capnp::MallocMessageBuilder& message);

            //ports::GroupSubscriberPort* FetchNextMessage(std::shared_ptr<capnp::FlatArrayMessageReader>& messageReader);
            void FetchNextMessage(zsock_t* in_socket);

//            bool SendMessageToLeader(capnp::MallocMessageBuilder& message);
//            bool SendLeaderMessage(capnp::MallocMessageBuilder& message);

            template<class T>
            riaps::ports::PortError SendToLeader(MessageBuilder<T>& message);

//            bool ProposeValueToLeader(capnp::MallocMessageBuilder &message, const std::string &proposeId);
//            bool SendVote(const std::string& propose_id, bool accept);
//
//            bool ProposeActionToLeader(const std::string& proposeId,
//                                       const std::string &actionId,
//                                       const timespec &absTime);

            const ComponentBase* parent_component() const;
            const std::string parent_component_id() const;

            std::shared_ptr<std::unordered_set<OwnId, OwnIdHasher>> GetKnownMembers();

            /**
             * Counts the records in _knownNodes map
             * Before counting, the DeleteTimeoutNodes() is called.
             * @return Number of nodes in the group.
             */
            uint16_t GetMemberCount();

            std::optional<OwnId> leader_id() const;

            std::shared_ptr<riaps::ports::GroupPublisherPort> group_pubport();
            std::shared_ptr<riaps::ports::GroupSubscriberPort> group_subport();
            std::shared_ptr<riaps::ports::QueryPort> group_qryport();
            std::shared_ptr<riaps::ports::AnswerPort> group_ansport();

            /**
             * Adjusted to python
             */
            double GetPythonNow();
            double GetPythonTime(timespec& now);
            void Heartbeat();

            riaps::utils::Timeout<std::chrono::milliseconds> last_heartbeat_;

            OwnId own_id_;

            const groups::GroupId& group_id() const;

            /**
             * List of ComponentID-s where the current group got HEARTBEAT messages from. Timestamped.
             *
             * @note The parent component is excluded from the list.
             *
             *  key   - component id (uuid, generated runtime, when the component starts
             *  value - timestamp of the last message from the given component
             */
            std::unordered_map<OwnId, riaps::utils::Timeout<std::chrono::milliseconds>,
                               OwnIdHasher,
                               OwnIdComparator> known_nodes_;

            template<class T>
            riaps::ports::PortError Send(MessageBuilder<T>& message);

            template<class T>
            riaps::ports::PortError SendToMember(MessageBuilder<T>& message, const std::string& identity);

            template<class T>
            std::optional<std::string> RequestVote(MessageBuilder<T>& topic_builder,
                                                   riaps::groups::poll::Voting kind = riaps::groups::poll::Voting::MAJORITY,
                                                   double timeout = 0.0);

            std::optional<std::string> RequestActionVote(const std::string& action,
                                                         const double when,        // Python representation of time
                                                         riaps::groups::poll::Voting kind = riaps::groups::poll::Voting::CONSENSUS,
                                                         double timeout = 0.0);

            template<class T>
            std::tuple<MessageReader<T>, riaps::ports::PortError> Recv();

            void SendVote(const std::string& rfvid, bool vote);

            /**
             * End adjusted
             */
            ~Group();
        private:
            /**
             * Delete records from the _knownNodes cache, it the Timer is exceeded
             * @return Number of deleted records.
             */
            uint32_t DeleteTimeoutNodes();

            GroupId     group_id_;
            const GroupConf* group_conf_;

            std::unique_ptr<MessageReaderArray> group_read_buffer_;

            /**
             * Always store the communication ports in shared_ptr
             */
            std::shared_ptr<riaps::ports::GroupPublisherPort>    group_pubport_;
            std::shared_ptr<riaps::ports::GroupSubscriberPort>   group_subport_;
            std::shared_ptr<riaps::ports::QueryPort>             group_qryport_;
            std::shared_ptr<riaps::ports::AnswerPort>            group_ansport_;

            std::shared_ptr<spd::logger> logger_;

            std::random_device random_device_;
            std::mt19937         random_generator_;
            std::uniform_int_distribution<int> timeout_distribution_;

            ComponentBase* parent_component_;

            std::unique_ptr<riaps::groups::GroupLead> group_leader_;

            void ProcessOnSub();
            void ProcessOnAns();
            void ProcessOnQry();

            bool pending_mfl_ = false;
            bool pending_mtl_ = false;
            bool pending_handle_vote_request_ = false;
        };

        template<class T>
        std::tuple<MessageReader<T>, riaps::ports::PortError> Group::Recv() {
            // if there is a message from the QRY port
            if (pending_mfl_) {
                auto[bytes, error] = group_qryport_->Recv();
                MessageReader<T> reader(bytes);
                return std::make_tuple(reader, error);
            } else if (pending_handle_vote_request_){
                // Pending message in the buffer
                MessageReader<T> reader(group_read_buffer_);
                return std::make_tuple(reader, riaps::ports::PortError(true));
            } else {
                // The message is on the SUB port
                auto[bytes, error] = group_subport_->Recv();
                MessageReader<T> reader(bytes);
                return std::make_tuple(reader, error);
            }
        };

        template<class T>
        riaps::ports::PortError Group::Send(MessageBuilder<T> &message) {
            capnp::MallocMessageBuilder& builder = message.capnp_builder();
            zframe_t* message_frame;
            message_frame << builder;
            zmsg_t* group_message = zmsg_new();
            zmsg_addstr(group_message, GROUP_MSG);
            zmsg_add(group_message, message_frame);
            return this->group_pubport_->Send(&group_message);
        }

        template<class T>
        riaps::ports::PortError Group::SendToMember(MessageBuilder<T> &message, const std::string& identity) {
            capnp::MallocMessageBuilder& builder = message.capnp_builder();
            zframe_t* message_frame;
            message_frame << builder;
            zmsg_t* group_message = zmsg_new();
            zmsg_addstr(group_message, identity.c_str());
            zmsg_addstr(group_message, GROUP_MFL);
            zmsg_add(group_message, message_frame);
            return this->group_ansport_->Send(&group_message);
        }

        template<class T>
        riaps::ports::PortError Group::SendToLeader(MessageBuilder<T> &message) {
            capnp::MallocMessageBuilder& builder = message.capnp_builder();
            zframe_t* message_frame;
            message_frame << builder;
            zmsg_t* leader_message = zmsg_new();
            zmsg_addstr(leader_message, GROUP_MTL);
            zmsg_add(leader_message, message_frame);
            return this->group_qryport_->Send(&leader_message);
        }

        template<class T>
        std::optional<std::string> Group::RequestVote(MessageBuilder<T> &topic_builder, riaps::groups::poll::Voting kind,
                                                      double timeout) {
            capnp::MallocMessageBuilder msg_builder;
            auto msg = msg_builder.initRoot<riaps::groups::poll::GroupVote>();
            auto rfv = msg.initRfv();
            rfv.setKind(kind);
            auto uid = zuuid_new();
            std::string uid_str = zuuid_str(uid);
            rfv.setRfvId(uid_str);
            rfv.setStarted(this->GetPythonNow());
            rfv.setSubject(riaps::groups::poll::Subject::VALUE);
            rfv.setTimeout(timeout/1000.0);

            auto topic_serialized = capnp::messageToFlatArray(topic_builder.capnp_builder());
            auto topic_bytes = topic_serialized.asBytes();

            rfv.initTopic(topic_bytes.size());
            rfv.setTopic(topic_bytes);

            zframe_t* message_frame;
            message_frame << msg_builder;
            zmsg_t* vote_message = zmsg_new();
            zmsg_addstr(vote_message, GROUP_RFV);
            zmsg_add(vote_message, message_frame);

            auto rc = group_qryport_->Send(&vote_message);

            zuuid_destroy(&uid);

            // Error
            if (rc) {
                return std::nullopt;
            }

            return uid_str;
        }
    }
}

#endif //RIAPS_CORE_R_GROUP_H_H

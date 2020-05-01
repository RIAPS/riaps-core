//
// Created by istvan on 12/18/17.
//

#ifndef RIAPS_CORE_R_GROUPLEAD_H
#define RIAPS_CORE_R_GROUPLEAD_H

#include <messaging/dc.capnp.h>
#include <groups/r_group.h>
#include <groups/r_groupdata.h>
#include <groups/r_ownid.h>
#include <spdlog_setup/conf.h>
#include <utils/r_timeout.h>
#include <czmq.h>
#include <functional>
#include <chrono>
#include <random>
#include <set>
#include <unordered_map>

//constexpr auto MIN_ELECTION_TIMEOUT = 150;
//constexpr auto MAX_ELECTION_TIMEOUT = 500;
//constexpr auto APPENDENTRY_TIMEOUT  = 75;

using namespace std::chrono;
using namespace riaps::utils;

namespace spd = spdlog;

namespace riaps{
    namespace groups{

        class Group;

        class GroupLead{
        public:
            /**
             * Follower: Expects hear form the leader. If no message arrives from the leader it becmoes a canidate.
             *
             *           [no message from leader]
             * FOLLOWER --------------------------> CANDIDATE
             *
             */
            enum NodeState{FOLLOWER, CANDIDATE, LEADER};
            GroupLead(riaps::groups::Group* group,
                    std::unordered_map<OwnId, riaps::utils::Timeout<std::chrono::milliseconds>,
                    OwnIdHasher,
                    OwnIdComparator>* known_nodes);
            const NodeState GetNodeState() const;

            void SetOnLeaderChanged(std::function<void(const std::string&)> handler);

            /**
             * Maintain the state when no incoming message.
             */
            void Update();

            std::optional<OwnId> GetLeaderId();

            /**
             * Incoming message arrived, lets update the state
             * @param internalMessage
             */
            //void Update(const char* command, zsock_t* socket);
            void UpdateReqVote(const riaps::groups::data::ReqVote& reqvote);
            void UpdateRspVote(const riaps::groups::data::RspVote& rspvote);
            void UpdateAuthority(const riaps::groups::data::Authority& authority);
            void SendAuthority();
            ~GroupLead();

            struct ProposeData {
                ProposeData(std::shared_ptr<std::set<std::string>> _knownNodes, Timeout<std::chrono::milliseconds>&& timeout);

                std::shared_ptr<std::set<std::string>> nodesInVote; // Expect vote from these nodes
                std::shared_ptr<std::set<std::string>> nodesVoted;  // ID-s of components which already sent the vote
                Timeout<std::chrono::milliseconds>     proposeDeadline; // If the propose expires, the leader announce REJECT
                uint16_t                               accepted;
                uint16_t                               rejected;

                bool                                   isAction;
                std::string                            actionId;
            };
        private:

            /**
             * Generates a random election timeout between 150ms and 300ms (values are specified in RAFT)
             * @return Random value between 150-300ms
             */
            int64_t GenerateElectionTimeo();

            std::random_device m_rd;
            std::mt19937 rnd_generator_;
            std::uniform_int_distribution<int> election_distr_;
            const OwnId& GetOwnId() const;

            NodeState current_state_;

            // Timeouts
            Timeout<std::chrono::milliseconds>  election_timeout_;
            Timeout<std::chrono::milliseconds>  appentry_timeout_;
            uint32_t election_term_;
            uint32_t number_of_nodes_in_vote_;

            std::string leader_address_;
            std::shared_ptr<riaps::ports::QueryPort>  group_qryport_;
            std::shared_ptr<riaps::ports::AnswerPort> group_ansport_;

            // Votes from, when
            std::unordered_map<OwnId, steady_clock::time_point, OwnIdHasher, OwnIdComparator> votes_;

            riaps::groups::Group* group_;

            // Send functions
            void SendRequestForVote();
            //void SendVote(const std::string& vote_for);
            void SendVote(uint32_t term, const OwnId &vote_for, bool vote);
            uint32_t GetNumberOfVotes();

            std::shared_ptr<spd::logger> logger_;
            std::unordered_map<OwnId, riaps::utils::Timeout<std::chrono::milliseconds>,
                    OwnIdHasher,
                    OwnIdComparator>* known_nodes_;

            std::optional<OwnId> leaderid_;

            void ChangeLeader(std::optional<OwnId> new_leader);
            std::function<void(const std::string&)> m_onLeaderChanged;


            /**
             * Active propseId-s with Timeouts, participating nodes and vote count
             */
            std::unordered_map<std::string, std::shared_ptr<riaps::groups::GroupLead::ProposeData>> m_proposeData;

            /**
             * Active ActionId-s
             */
            std::unordered_map<std::string, std::shared_ptr<riaps::groups::GroupLead::ProposeData>> m_actionData;
        };
    }
}

#endif //RIAPS_CORE_R_GROUPLEAD_H

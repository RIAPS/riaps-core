//
// Created by istvan on 12/18/17.
//

#ifndef RIAPS_CORE_R_GROUPLEAD_H
#define RIAPS_CORE_R_GROUPLEAD_H

#include <messaging/distcoord.capnp.h>
#include <groups/r_group.h>
#include <spdlog/spdlog.h>
#include <utils/r_timeout.h>
#include <czmq.h>
#include <functional>
#include <chrono>
#include <random>
#include <set>
#include <unordered_map>

#define MIN_ELECTION_TIMEOUT 150
#define MAX_ELECTION_TIMEOUT 500
#define APPENDENTRY_TIMEOUT  75

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
            GroupLead(riaps::groups::Group* group, std::unordered_map<std::string, Timeout<std::chrono::milliseconds>>* knownNodes);
            const NodeState GetNodeState() const;

            void SetOnLeaderChanged(std::function<void(const std::string&)> handler);

            /**
             * Maintain the state whan no incoming message.
             */
            void Update();

            void OnProposeFromClient(riaps::distrcoord::Consensus::ProposeToLeader::Reader& headerMessage,
                                     zframe_t** messageFrame);

            void OnActionProposeFromClient(riaps::distrcoord::Consensus::ProposeToLeader::Reader& headerMessage,
                                           riaps::distrcoord::Consensus::TimeSyncCoordA::Reader&  tscaMessage);

            void OnVote(riaps::distrcoord::Consensus::Vote::Reader& message,
                        const std::string& sourceComponentId);

            std::string GetLeaderId();

            /**
             * Incoming message arrived, lets update the state
             * @param internalMessage
             */
            void Update(riaps::distrcoord::LeaderElection::Reader& internalMessage);
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
            std::mt19937 m_generator;
            std::uniform_int_distribution<int> m_distrElection;
            const std::string GetComponentId() const;

            NodeState m_currentState;

            // Timeouts
            Timeout<std::chrono::milliseconds>  m_electionTimeout;
            Timeout<std::chrono::milliseconds>  m_appEntryTimeout;
            uint32_t m_electionTerm;
            uint32_t m_numberOfNodesInVote;



            // Votes from, when
            std::unordered_map<std::string, steady_clock::time_point> m_votes;

            riaps::groups::Group* m_group;


            // Send functions
            void SendRequestForVote();
            void SendAppendEntry();
            void SendVote(const std::string& voteFor);
            void Announce(const std::string& proposeId, riaps::distrcoord::Consensus::VoteResults result);
            uint32_t GetNumberOfVotes();

            std::shared_ptr<spd::logger> _logger;
            std::unordered_map<std::string, Timeout<std::chrono::milliseconds>>* m_knownNodes;

            std::string m_leaderId;

            void ChangeLeader(const std::string& newLeader);
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

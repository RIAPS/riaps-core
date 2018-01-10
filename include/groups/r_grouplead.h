//
// Created by istvan on 12/18/17.
//

#ifndef RIAPS_CORE_R_GROUPLEAD_H
#define RIAPS_CORE_R_GROUPLEAD_H

#include <messaging/distcoord.capnp.h>
#include <groups/r_group.h>
#include <spdlog/spdlog.h>
#include <utils/r_timeout.h>

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
            GroupLead(riaps::groups::Group* group, std::unordered_map<std::string, Timeout>* knownNodes);
            const NodeState GetNodeState() const;

            void SetOnLeaderChanged(std::function<void(const std::string&)> handler);

            /**
             * Maintain the state whan no incoming message.
             */
            void Update();

            void ProposeFromClient(riaps::distrcoord::DistrCoord::ProposeToLeader::Reader& message);
            void OnVote(riaps::distrcoord::DistrCoord::VoteForLeader::Reader& message,
                        const std::string& sourceComponentId);

            std::string GetLeaderId();

            /**
             * Incoming message arrived, lets update the state
             * @param internalMessage
             */
            void Update(riaps::distrcoord::LeaderElection::Reader& internalMessage);
            ~GroupLead();

            struct ProposeData {
                ProposeData(std::shared_ptr<std::set<std::string>> _knownNodes, Timeout&& timeout);

                std::shared_ptr<std::set<std::string>> nodesInVote; // Expect vote from these nodes
                std::shared_ptr<std::set<std::string>> nodesVoted;  // ID-s of components which already sent the vote
                Timeout               proposeDeadline; // If the propose expires, the leader announce REJECT
            };
        private:

            /**
             * Generates a random election timeout between 150ms and 300ms (values are specified in RAFT)
             * @return Random value between 150-300ms
             */
            duration<int, std::milli> GenerateElectionTimeo();

            std::random_device _rd;
            std::mt19937 _generator;
            std::uniform_int_distribution<int> _distrElection;
            const std::string GetComponentId() const;

            NodeState _currentState;

            // Timeouts
            Timeout  _electionTimeout;
            Timeout  _appEntryTimeout;
            uint32_t _electionTerm;
            uint32_t _numberOfNodesInVote;



            // Votes from, when
            std::unordered_map<std::string, steady_clock::time_point> _votes;

            riaps::groups::Group* _group;


            // Send functions
            void SendRequestForVote();
            void SendAppendEntry();
            void SendVote(const std::string& voteFor);
            void Announce(const std::string& proposeId, riaps::distrcoord::DistrCoord::VoteResults result);
            uint32_t GetNumberOfVotes();

            std::shared_ptr<spd::logger> _logger;
            std::unordered_map<std::string, Timeout>* _knownNodes;

            std::string _leaderId;

            void ChangeLeader(const std::string& newLeader);
            std::function<void(const std::string&)> _onLeaderChanged;


            /**
             * Active propseId-s with Timeouts, participating nodes and vote count
             */
            std::unordered_map<std::string, std::unique_ptr<riaps::groups::GroupLead::ProposeData>> _proposeData;


        };
    }
}

#endif //RIAPS_CORE_R_GROUPLEAD_H

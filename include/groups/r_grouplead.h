//
// Created by istvan on 12/18/17.
//

#ifndef RIAPS_CORE_R_GROUPLEAD_H
#define RIAPS_CORE_R_GROUPLEAD_H

#include <messaging/distcoord.capnp.h>
#include <groups/r_group.h>
#include <functional>
#include <chrono>
#include <random>

#define MIN_ELECTION_TIMEOUT 150
#define MAX_ELECTION_TIMEOUT 500
#define APPENDENTRY_TIMEOUT  75

using namespace std::chrono;

namespace riaps{
    namespace groups{

        class Group;

        class Timeout {
        public:

            /**
             * Intitalizes with 0 timeout from now;
             */
            Timeout();

            /**
             * Initializes the timeut structure from ::now() with the passed timeout
             * @param timeout
             */
            Timeout(duration<int, std::milli> timeout);

            /**
             * Resets the start time point, doesn't touch the timeout
             */
            void Reset();

            /**
             * Resets the start point and the timeout value
             * @param timeout
             */
            void Reset(duration<int, std::milli> timeout);

            /**
             * If ::now()>_endPoint
             * @return
             */
            bool IsTimeout();

            steady_clock::time_point GetEndTimePoint();

            ~Timeout();
        private:
            steady_clock::time_point  _startPoint; // The election timeout from this timepoint
            duration<int, std::milli> _timeout; // The election timeout
            steady_clock::time_point  _endPoint;
        };

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
            GroupLead(riaps::groups::Group* group);
            const NodeState GetNodeState() const;

            /**
             * Maintain the state whan no incoming message.
             */
            void Update();

            /**
             * Incoming message arrived, lets update the state
             * @param internalMessage
             */
            void Update(riaps::distrcoord::LeaderElection::Reader& internalMessage);
            ~GroupLead();
        private:

            /**
             * Generates a random election timeout between 150ms and 300ms (values are specified in RAFT)
             * @return Random value between 150-300ms
             */
            duration<int, std::milli> GenerateElectionTimeo();

            //std::random_device _rd;
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
            uint32_t GetNumberOfVotes();


        };
    }
}

#endif //RIAPS_CORE_R_GROUPLEAD_H

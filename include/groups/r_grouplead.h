//
// Created by istvan on 12/18/17.
//

#ifndef RIAPS_CORE_R_GROUPLEAD_H
#define RIAPS_CORE_R_GROUPLEAD_H

#include <groups/r_group.h>

#include <chrono>

using namespace std::chrono;

namespace riaps{
    namespace groups{

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
            GroupLead();
            const NodeState GetNodeState() const;
            void Step();
            ~GroupLead();
        private:

            void ResetTimer(steady_clock::time_point& start, steady_clock::time_point& end, const duration<int, std::milli>& period);
            bool IsInPeriod(const steady_clock::time_point& start, const steady_clock::time_point& end, const steady_clock::time_point& sample) const;

            NodeState _currentState;

            steady_clock::time_point  _waitTimeStart;
            duration<int, std::milli> _waitPeriod;
            steady_clock::time_point  _waitTimeEnd;

            std::function<void()> _resetWaitTime;

            std::function<void()> _resetVoteTime;


            steady_clock::time_point _lastMessageFromLeader;


            uint32_t                  _numberOfNodesInVote;
            steady_clock::time_point  _voteStart;
            steady_clock::time_point  _voteEnd;
            duration<int, std::milli> _votePeriod;

        };
    }
}

#endif //RIAPS_CORE_R_GROUPLEAD_H

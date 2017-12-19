//
// Created by istvan on 12/18/17.
//

#ifndef RIAPS_CORE_R_GROUPLEAD_H
#define RIAPS_CORE_R_GROUPLEAD_H

#include <messaging/distcoord.capnp.h>
#include <groups/r_group.h>
#include <functional>
#include <chrono>

using namespace std::chrono;

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
            GroupLead(riaps::groups::Group* parentGroup);
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

            void ResetTimer(steady_clock::time_point& start, steady_clock::time_point& end, const duration<int, std::milli>& period);
            bool IsInPeriod(const steady_clock::time_point& start, const steady_clock::time_point& end, const steady_clock::time_point& sample) const;

            NodeState _currentState;

            // Last time when the group heard about the leader
            steady_clock::time_point  _waitTimeStart;
            duration<int, std::milli> _waitPeriod;
            steady_clock::time_point  _waitTimeEnd;

            std::function<void()> _resetWaitTime;

            std::function<void()> _resetVoteTime;



            uint32_t                  _numberOfNodesInVote;
            steady_clock::time_point  _voteStart;
            steady_clock::time_point  _voteEnd;
            duration<int, std::milli> _votePeriod;

            riaps::groups::Group* _parentGroup;
        };
    }
}

#endif //RIAPS_CORE_R_GROUPLEAD_H

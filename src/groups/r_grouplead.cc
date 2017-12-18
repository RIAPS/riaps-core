//
// Created by istvan on 12/18/17.
//

#include <groups/r_grouplead.h>


using namespace riaps::groups;
using namespace std::chrono;

GroupLead::GroupLead() {
    /**
     * In RAFT everybody starts in the FOLLOWER state
     */
    _currentState = NodeState::FOLLOWER;

    /**
     * Set up the reset functions
     */

    _resetWaitTime = std::bind(&GroupLead::ResetTimer, this, _waitTimeStart, _waitTimeEnd, _waitPeriod);
    _resetVoteTime = std::bind(&GroupLead::ResetTimer, this, _voteStart, _voteEnd, _votePeriod);

    /**
     * Wait period is 300ms
     * Vote period is 300ms
     * TODO: Check if it should be random
     * TODO: Check lowest and highest in RAFT specification
     */
    _waitPeriod = milliseconds(300);
    _votePeriod = milliseconds(300);

    /**
    * Initialize the wait time interval and the last message from the leader with the current timestamp
    */
    _resetWaitTime();
    _lastMessageFromLeader = _waitTimeStart;
}

void GroupLead::Step() {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    /**
     * If the node is FOLLOWER and the wait time is not expired, then do nothing
     */
    if (_currentState == GroupLead::NodeState::FOLLOWER && IsInPeriod(_waitTimeStart, _waitTimeEnd, now)){
        return;
    }

    /**
     * The node is a FOLLOWER but the wait time is expired without hearing from the leader
     * FOLLOWER --> CANDIDATE
     */
    else if (_currentState == GroupLead::NodeState::FOLLOWER && _lastMessageFromLeader>_waitTimeEnd) {
        // TODO: what ha[[ens with the wait time? Maybe doesn't matter when the node is candidate

        /**
         * Step into the next state and send REQUEST_VOTE message to everybody in the group
         * The number of nodes are saved since the MAJORITY of votes is needed
         */
        _currentState = GroupLead::NodeState::CANDIDATE;
        _resetVoteTime();

        // TODO: Get the number of nodes in the group
        // _numberOfNodesInVote = ?

        // TODO: SendMessage()
    }

}


bool GroupLead::IsInPeriod(const steady_clock::time_point& start,
                           const steady_clock::time_point& end,
                           const steady_clock::time_point& sample) const {
    return sample>=start && sample<=end;
}

void GroupLead::ResetTimer(steady_clock::time_point& start,
                           steady_clock::time_point& end,
                           const duration<int, std::milli>& period) {
    start = steady_clock::now();
    end   = start + period;
}

const GroupLead::NodeState GroupLead::GetNodeState() const {
    return _currentState;
}

GroupLead::~GroupLead() {

}
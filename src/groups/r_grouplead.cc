//
// Created by istvan on 12/18/17.
//

#include <groups/r_grouplead.h>


using namespace riaps::groups;
using namespace std::chrono;

Timeout::Timeout() {
    Reset(duration<int, std::milli>(0));
}

Timeout::Timeout(duration<int, std::milli> timeout) {
    Reset(timeout);
}

void Timeout::Reset() {
    _startPoint = steady_clock::now();
    _endPoint   = _startPoint + _timeout;
}

void Timeout::Reset(duration<int, std::milli> timeout) {
    _timeout = timeout;
    Reset();
}

steady_clock::time_point Timeout::GetEndTimePoint() {
    return _endPoint;
}

bool Timeout::IsTimeout() {
    auto now = steady_clock::now();
    return now>_endPoint;
}

Timeout::~Timeout() {

}

GroupLead::GroupLead(Group* group)
    : _group(group)
{

    /**
     * Initialize random generators
     */
    _generator        = std::mt19937(0);
    _distrElection    = std::uniform_int_distribution<int>(MIN_ELECTION_TIMEOUT, MAX_ELECTION_TIMEOUT);
    _electionTimeout  = Timeout(GenerateElectionTimeo());
    _appEntryTimeout  = Timeout(duration<int, std::milli>(APPENDENTRY_TIMEOUT));
    _electionTerm     = 0;

    /**
     * In RAFT everybody starts in the FOLLOWER state
     */
    _currentState = NodeState::FOLLOWER;
}

void GroupLead::Update() {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    /**
     * If the node is FOLLOWER and the wait time is not expired, then do nothing
     */
    if (_currentState == GroupLead::NodeState::FOLLOWER && !_electionTimeout.IsTimeout()){
        return;
    }

    /**
     * The node is a FOLLOWER but the wait time is expired without hearing from the leader
     * FOLLOWER --> CANDIDATE
     */
    else if (_currentState == GroupLead::NodeState::FOLLOWER && _electionTimeout.IsTimeout()) {
        /**
         * Step into the next state and send REQUEST_VOTE message to everybody in the group
         * The number of nodes are saved since the MAJORITY of votes is needed
         */
        _currentState = GroupLead::NodeState::CANDIDATE;

        /**
         * Snapshot of members in the last MAX_ELECTION_TIMEOUTms.
         * TODO: We chose MAX_ELECTION_TIMEOUTms because this is the max leader election timeout. Should we reconsider this value?
         */
        _numberOfNodesInVote = _group->GetMemberCount(MAX_ELECTION_TIMEOUT /*msec*/);

        /**
         * Set the first election Term
         */
        _electionTerm++;

        /**
         * The node votes to itself, save the timestamp to filter possibly old votes
         */
        _votes.clear();
        _votes[GetComponentId()] = now;

        // Note: What to do when only one node is in the group??? This is not trivial. RAFT doesn't work with one node.
        // But RIAPS should does.
        SendRequestForVote();

        /**
         * Waiting for responses until the MAX election timeout.
         */
        _electionTimeout.Reset(duration<int, std::milli>(MAX_ELECTION_TIMEOUT));
    }

    /**
     * If the node is candidate and the wait time (vote time when candidate) has expired
     * It means that nobody responded to the vote request.
     */
    else if (_currentState == GroupLead::CANDIDATE && !_electionTimeout.IsTimeout()){

    } else if (_currentState == GroupLead::CANDIDATE && _electionTimeout.IsTimeout()){
        // Vote still in progress, but the timeout has expired.
        // Be a follower again, maybe better luck next time.

        _currentState = GroupLead::NodeState::FOLLOWER;
//        _numberOfNodesInVote = _group->GetMemberCount(MAX_ELECTION_TIMEOUT /*msec*/);
//        _electionTerm++;
//        _votes.clear();
//        _votes[GetComponentId()] = now;
//        SendRequestForVote();
        _electionTimeout.Reset(duration<int, std::milli>(MAX_ELECTION_TIMEOUT));
    }

    /**
     * If no incoming message for the leader, then the leader just heartbeating
     */
    else if (_currentState == GroupLead::LEADER && _appEntryTimeout.IsTimeout()) {
        SendAppendEntry();
        _appEntryTimeout.Reset();
    }
}

const std::string GroupLead::GetComponentId() const {
    return _group->GetParentComponent()->GetCompUuid();
}

void GroupLead::Update(riaps::distrcoord::LeaderElection::Reader &internalMessage) {
    auto now = steady_clock::now();

    /**
     * RequestForVote request arrived and the node is a follower.
     */
    if (internalMessage.hasRequestForVoteReq() && _currentState == GroupLead::NodeState::FOLLOWER) {
        auto msgVoteReq = internalMessage.getRequestForVoteReq();
        auto electTerm  = msgVoteReq.getElectionTerm();
        std::string sourceCompId = msgVoteReq.getSourceComponentId();

        /**
         * If the component didn't vote in this round (term) and the sender is a different component then VOTE
         */
        if (_electionTerm<electTerm && sourceCompId!=GetComponentId()){
            _electionTerm = electTerm;
            _electionTimeout.Reset(GenerateElectionTimeo());
            SendVote(sourceCompId);
        }
    }
    else if (internalMessage.hasRequestForVoteReq() && _currentState == GroupLead::NodeState::CANDIDATE){
        // This is tricky. Looks like somebody else also started a vote.
        // Selfish, behavior but understandable the candidate doesn't vote for another candidate.

        auto msgVoteReq = internalMessage.getRequestForVoteReq();
        auto electTerm  = msgVoteReq.getElectionTerm();
        std::string sourceCompId = msgVoteReq.getSourceComponentId();

        // TODO: Evaluate what happens when the electTerm>_electionTerm (the reqeust is for a newever vote)

    } else if (internalMessage.hasRequestForVoteReq() && _currentState == GroupLead::NodeState::LEADER){
        /**
         * Do nothing. This guy is already a leader.
         */
    }

    /**
     * RequestForVote response arrived
     */
    else if (internalMessage.hasRequestForVoteRep() && _currentState == CANDIDATE) {
        auto        theVote  = internalMessage.getRequestForVoteRep();
        std::string votedTo  = theVote.getVoteForId();
        std::string voteFrom = theVote.getSourceComponentId();
        auto        elecTerm = theVote.getElectionTerm();

        /**
         * Accept the vote only if the vote is for the current term and the From is not the current node
         */
        if (GetComponentId() != voteFrom && elecTerm == _electionTerm && votedTo == GetComponentId()) {
            _votes[theVote.getSourceComponentId().cStr()] = steady_clock::now();

            bool hasMajority = false;
            if (_numberOfNodesInVote%2 == 0){
                uint32_t majority = (_numberOfNodesInVote/2)+1;
                if (majority<=GetNumberOfVotes())
                    hasMajority = true;
            } else if (_numberOfNodesInVote%2==1){
                uint32_t majority = ceil(_numberOfNodesInVote/2.0);
                if (majority<=GetNumberOfVotes())
                    hasMajority = true;
            }

            /**
             * MAJORITY ACHIVED, send append entry, switch state
             */
            if (hasMajority){
                _currentState = GroupLead::LEADER;
                SendAppendEntry();
            }
        }
    } else if (internalMessage.hasAppendEntry() && _currentState==GroupLead::FOLLOWER){
        _electionTimeout.Reset(GenerateElectionTimeo());
    }
}

// TODO: check timestamps
uint32_t GroupLead::GetNumberOfVotes() {
    return _votes.size();
}

duration<int, std::milli> GroupLead::GenerateElectionTimeo() {
    return duration<int, std::milli>(_distrElection(_generator));
}

const GroupLead::NodeState GroupLead::GetNodeState() const {
    return _currentState;
}

void GroupLead::SendRequestForVote() {
    capnp::MallocMessageBuilder requestForVoteBuilder;
    auto msgInternals  = requestForVoteBuilder.initRoot<riaps::distrcoord::GroupInternals>();
    auto msgLeader     = msgInternals.initLeaderElection();
    auto msgReqForVote = msgLeader.initRequestForVoteReq();
    msgReqForVote.setSourceComponentId(GetComponentId());
    msgReqForVote.setElectionTerm(_electionTerm);

    _group->SendInternalMessage(requestForVoteBuilder);
}

void GroupLead::SendAppendEntry() {
    capnp::MallocMessageBuilder appendEntryBuilder;
    auto msgInternals  = appendEntryBuilder.initRoot<riaps::distrcoord::GroupInternals>();
    auto msgLeader     = msgInternals.initLeaderElection();
    auto msgAppendEntry = msgLeader.initAppendEntry();
    msgAppendEntry.setSourceComponentId(GetComponentId());
    msgAppendEntry.setElectionTerm(_electionTerm);

    _group->SendInternalMessage(appendEntryBuilder);
}

void GroupLead::SendVote(const std::string& voteFor) {
    capnp::MallocMessageBuilder voteBuilder;
    auto msgInternals = voteBuilder.initRoot<riaps::distrcoord::GroupInternals>();
    auto msgLeader = msgInternals.initLeaderElection();
    auto msgVote = msgLeader.initRequestForVoteRep();
    msgVote.setElectionTerm(_electionTerm);
    msgVote.setSourceComponentId(GetComponentId());
    msgVote.setVoteForId(voteFor);

    _group->SendInternalMessage(voteBuilder);
}

GroupLead::~GroupLead() {

}
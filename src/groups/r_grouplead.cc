//
// Created by istvan on 12/18/17.
//

#include <groups/r_grouplead.h>


using namespace riaps::groups;
using namespace std::chrono;


GroupLead::GroupLead(Group* group, std::unordered_map<std::string, Timeout>* knownNodes)
    : _group(group),
      _knownNodes(knownNodes)
{

    /**
     * Initialize random generators
     */
    _generator        = std::mt19937(_rd());
    _distrElection    = std::uniform_int_distribution<int>(MIN_ELECTION_TIMEOUT, MAX_ELECTION_TIMEOUT);
    _electionTimeout  = Timeout(GenerateElectionTimeo());
    _appEntryTimeout  = Timeout(duration<int, std::milli>(APPENDENTRY_TIMEOUT));
    _electionTerm     = 0;

    /**
     * In RAFT everybody starts in the FOLLOWER state
     */
    _currentState = NodeState::FOLLOWER;

    _logger = spd::get(group->GetParentComponent()->GetConfig().component_name);
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
         * Note: +1 is added becuase the current node is not included in the GroupMemberCount();
         */
        _numberOfNodesInVote = _group->GetMemberCount() + 1;

        /**
         * Set the first election Term
         */
        _electionTerm++;
        //_logger->debug("[{}] Election timeout FOLLOWER->CANDIDATE", _electionTerm);

        /**
         * The node votes to itself, save the timestamp to filter possibly old votes
         */
        _votes.clear();
        _votes[GetComponentId()] = now;

        // TODO: What to do when only one node is in the group??? This is not trivial. RAFT doesn't work with one node.
        // But RIAPS should?
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
        // Vote is still in progress, but the timeout has expired.
        // Be a follower again, maybe better luck next time.

        //_logger->debug("[{}] Vote timeout CANDIDATE->FOLLOWER",_electionTerm);

        _currentState = GroupLead::NodeState::FOLLOWER;
        _electionTimeout.Reset(duration<int, std::milli>(MAX_ELECTION_TIMEOUT));
    }

    /**
     * If no incoming message for the leader, then the leader just heartbeating
     * Note: The receivers don't respond to AppendEndtry (no log replication in RIAPS so far,
     * no reason to implement the reply)
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
        //(*_knownNodes)[sourceCompId] = zclock_mono();
        
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
        //(*_knownNodes)[sourceCompId] = zclock_mono();

        // If higher term arrived switch state back to follower
        // TODO: And vote?
        if (electTerm>_electionTerm){
            _currentState = GroupLead::NodeState::FOLLOWER;
            _electionTimeout.Reset(GenerateElectionTimeo());
            SendVote(sourceCompId);
        }

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
        //(*_knownNodes)[voteFrom] = zclock_mono();

        /**
         * Accept the vote only if the vote is for the current term and the From is not the current node
         */
        if (GetComponentId() != voteFrom && elecTerm == _electionTerm && votedTo == GetComponentId()) {
            _votes[theVote.getSourceComponentId().cStr()] = steady_clock::now();

            bool hasMajority = false;
            if (_numberOfNodesInVote%2 == 0){
                uint32_t majority = (_numberOfNodesInVote/2)+1;
                auto numberOfVotes = GetNumberOfVotes();
                if (majority<=numberOfVotes)
                    hasMajority = true;
                else
                    _logger->debug("No majority ({}): {}/{}",majority,_numberOfNodesInVote,numberOfVotes);
            } else if (_numberOfNodesInVote%2==1){
                uint32_t majority = ceil(_numberOfNodesInVote/2.0);
                auto numberOfVotes = GetNumberOfVotes();
                if (majority<=numberOfVotes)
                    hasMajority = true;
                else
                    _logger->debug("No majority ({}): {}/{}",majority,_numberOfNodesInVote,numberOfVotes);
            }

            /**
             * MAJORITY ACHIEVED, send append entry, switch state
             */
            if (hasMajority){
                _currentState = GroupLead::LEADER;
                SendAppendEntry();
            }
        }
    } else if (internalMessage.hasAppendEntry() && _currentState==GroupLead::FOLLOWER){
        auto msgAppendEntry = internalMessage.getAppendEntry();
        //(*_knownNodes)[msgAppendEntry.getSourceComponentId().cStr()] = zclock_mono();
        //_logger->debug("Append entry from: {0}", msgAppendEntry.getSourceComponentId().cStr());
        _electionTimeout.Reset(GenerateElectionTimeo());
        _electionTerm = msgAppendEntry.getElectionTerm();
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
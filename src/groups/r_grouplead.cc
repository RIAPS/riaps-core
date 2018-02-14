//
// Created by istvan on 12/18/17.
//

#include <groups/r_grouplead.h>


using namespace riaps::groups;
using namespace std::chrono;


GroupLead::GroupLead(Group* group, std::unordered_map<std::string, Timeout>* knownNodes)
    : m_group(group),
      m_knownNodes(knownNodes)
{

    /**
     * Initialize random generators
     */
    m_generator        = std::mt19937(m_rd());
    m_distrElection    = std::uniform_int_distribution<int>(MIN_ELECTION_TIMEOUT, MAX_ELECTION_TIMEOUT);
    m_electionTimeout  = Timeout(GenerateElectionTimeo());
    m_appEntryTimeout  = Timeout(duration<int, std::milli>(APPENDENTRY_TIMEOUT));
    m_electionTerm     = 0;

    /**
     * In RAFT everybody starts in the FOLLOWER state
     */
    m_currentState = NodeState::FOLLOWER;

    _logger = spd::get(group->GetParentComponent()->GetConfig().component_name);
}

std::string GroupLead::GetLeaderId() {
    return m_leaderId;
}

void GroupLead::Update() {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    /**
     * If the node is FOLLOWER and the wait time is not expired, then do nothing
     */
    if (m_currentState == GroupLead::NodeState::FOLLOWER && !m_electionTimeout.IsTimeout()){
        return;
    }

    /**
     * The node is a FOLLOWER but the wait time is expired without hearing from the leader
     * FOLLOWER --> CANDIDATE
     */
    else if (m_currentState == GroupLead::NodeState::FOLLOWER && m_electionTimeout.IsTimeout()) {
        ChangeLeader("");
        /**
         * Step into the next state and send REQUEST_VOTE message to everybody in the group
         * The number of nodes are saved since the MAJORITY of votes is needed
         */
        m_currentState = GroupLead::NodeState::CANDIDATE;


        /**
         * Note: +1 is added becuase the current node is not included in the GroupMemberCount();
         */
        m_numberOfNodesInVote = m_group->GetMemberCount() + 1;

        /**
         * Set the first election Term
         */
        m_electionTerm++;
        //_logger->debug("[{}] Election timeout FOLLOWER->CANDIDATE", _electionTerm);

        /**
         * The node votes to itself, save the timestamp to filter possibly old votes
         */
        m_votes.clear();
        m_votes[GetComponentId()] = now;

        // TODO: What to do when only one node is in the group??? This is not trivial. RAFT doesn't work with one node.
        // But RIAPS should?
        SendRequestForVote();

        /**
         * Waiting for responses until the MAX election timeout.
         */
        m_electionTimeout.Reset(duration<int, std::milli>(MAX_ELECTION_TIMEOUT));
    }

    /**
     * If the node is candidate and the wait time (vote time when candidate) has expired
     * It means that nobody responded to the vote request.
     */
    else if (m_currentState == GroupLead::CANDIDATE && !m_electionTimeout.IsTimeout()){

    } else if (m_currentState == GroupLead::CANDIDATE && m_electionTimeout.IsTimeout()){
        // Vote is still in progress, but the timeout has expired.
        // Be a follower again, maybe better luck next time.

        //_logger->debug("[{}] Vote timeout CANDIDATE->FOLLOWER",_electionTerm);

        m_currentState = GroupLead::NodeState::FOLLOWER;
        m_electionTimeout.Reset(duration<int, std::milli>(MAX_ELECTION_TIMEOUT));
    }

    /**
     * If no incoming message for the leader, then the leader just heartbeating
     * Note: The receivers don't respond to AppendEndtry (no log replication in RIAPS so far,
     * no reason to implement the reply)
     */
    else if (m_currentState == GroupLead::LEADER && m_appEntryTimeout.IsTimeout()) {
        SendAppendEntry();
        m_appEntryTimeout.Reset();
        m_leaderId = GetComponentId();
    }
    /**
     * If no incoming message for the leader and no timeout for sending heartbeat, then maintain the things.
     *  like: Checking ongoing vote timeouts
     */
    else if (m_currentState == GroupLead::LEADER && !m_appEntryTimeout.IsTimeout()) {

        // TODO: Do it periodically and not in every cycle?
        for (auto it = m_proposeData.begin(); it!=m_proposeData.end();){
            if (it->second->proposeDeadline.IsTimeout()){
                Announce(it->first, riaps::distrcoord::Consensus::VoteResults::REJECTED);
                it = m_proposeData.erase(it);
            } else {
                it++;
            }
        }
    }
}

const std::string GroupLead::GetComponentId() const {
    return m_group->GetParentComponent()->GetCompUuid();
}

void GroupLead::Update(riaps::distrcoord::LeaderElection::Reader &internalMessage) {
    auto now = steady_clock::now();

    /**
     * RequestForVote request arrived and the node is a follower.
     */
    if (internalMessage.hasRequestForVoteReq() && m_currentState == GroupLead::NodeState::FOLLOWER) {
        auto msgVoteReq = internalMessage.getRequestForVoteReq();
        auto electTerm  = msgVoteReq.getElectionTerm();
        std::string sourceCompId = msgVoteReq.getSourceComponentId();
        //(*_knownNodes)[sourceCompId] = zclock_mono();
        
        /**
         * If the component didn't vote in this round (term) and the sender is a different component then VOTE
         */
        if (m_electionTerm<electTerm && sourceCompId!=GetComponentId()){
            m_electionTerm = electTerm;
            m_electionTimeout.Reset(GenerateElectionTimeo());
            SendVote(sourceCompId);


        }
    }
    else if (internalMessage.hasRequestForVoteReq() &&
            (m_currentState == GroupLead::NodeState::CANDIDATE || m_currentState == GroupLead::NodeState::LEADER)){
        // This is tricky. Looks like somebody else also started a vote.
        // Selfish, behavior but understandable the candidate doesn't vote for another candidate.

        auto msgVoteReq = internalMessage.getRequestForVoteReq();
        auto electTerm  = msgVoteReq.getElectionTerm();
        std::string sourceCompId = msgVoteReq.getSourceComponentId();
        //(*_knownNodes)[sourceCompId] = zclock_mono();

        // If higher term arrived switch state back to follower
        // TODO: And vote?
        if (electTerm>m_electionTerm){
            m_currentState = GroupLead::NodeState::FOLLOWER;
            m_electionTerm = electTerm;

            if (m_currentState == GroupLead::NodeState::LEADER){
                ChangeLeader("");
            }

            m_electionTimeout.Reset(GenerateElectionTimeo());
            SendVote(sourceCompId);
        }

    } //else if (internalMessage.hasRequestForVoteReq() && _currentState == GroupLead::NodeState::LEADER){
        /**
         * Do nothing. This guy is already a leader.
         */
    //}

    /**
     * RequestForVote response arrived
     */
    else if (internalMessage.hasRequestForVoteRep() && m_currentState == CANDIDATE) {
        auto        theVote  = internalMessage.getRequestForVoteRep();
        std::string votedTo  = theVote.getVoteForId();
        std::string voteFrom = theVote.getSourceComponentId();
        auto        elecTerm = theVote.getElectionTerm();
        //(*_knownNodes)[voteFrom] = zclock_mono();

        /**
         * Accept the vote only if the vote is for the current term and the From is not the current node
         */
        if (GetComponentId() != voteFrom && elecTerm == m_electionTerm && votedTo == GetComponentId()) {
            m_votes[theVote.getSourceComponentId().cStr()] = steady_clock::now();

            bool hasMajority = false;
            if (m_numberOfNodesInVote%2 == 0){
                uint32_t majority = (m_numberOfNodesInVote/2)+1;
                auto numberOfVotes = GetNumberOfVotes();
                if (majority<=numberOfVotes)
                    hasMajority = true;
                else
                    _logger->debug("No majority ({}): {}/{}",majority,m_numberOfNodesInVote,numberOfVotes);
            } else if (m_numberOfNodesInVote%2==1){
                uint32_t majority = ceil(m_numberOfNodesInVote/2.0);
                auto numberOfVotes = GetNumberOfVotes();
                if (majority<=numberOfVotes)
                    hasMajority = true;
                else
                    _logger->debug("No majority ({}): {}/{}",majority,m_numberOfNodesInVote,numberOfVotes);
            }

            /**
             * MAJORITY ACHIEVED, send append entry, switch state
             */
            if (hasMajority){
                m_currentState = GroupLead::LEADER;
                SendAppendEntry();
                ChangeLeader(GetComponentId());
            }
        }
    } else if (internalMessage.hasAppendEntry() && m_currentState==GroupLead::FOLLOWER){
        auto msgAppendEntry = internalMessage.getAppendEntry();
        //(*_knownNodes)[msgAppendEntry.getSourceComponentId().cStr()] = zclock_mono();
        //_logger->debug("Append entry from: {0}", msgAppendEntry.getSourceComponentId().cStr());
        m_electionTimeout.Reset(GenerateElectionTimeo());
        m_electionTerm = msgAppendEntry.getElectionTerm();
        ChangeLeader(msgAppendEntry.getSourceComponentId().cStr());
    } else if (internalMessage.hasAppendEntry() && m_currentState == GroupLead::LEADER){
        /**
         * Something went wrong, cannot be two leader, the leader with less election term will be a follower
         */
        auto msgAppendEntry = internalMessage.getAppendEntry();
        auto electTerm = msgAppendEntry.getElectionTerm();

        if (electTerm>m_electionTerm){
            m_currentState = GroupLead::NodeState::FOLLOWER;
            m_electionTerm = electTerm;
            ChangeLeader(msgAppendEntry.getSourceComponentId());
            m_electionTimeout.Reset(GenerateElectionTimeo());
        }
    }
}

// TODO: check timestamps
uint32_t GroupLead::GetNumberOfVotes() {
    return m_votes.size();
}

duration<int, std::milli> GroupLead::GenerateElectionTimeo() {
    return duration<int, std::milli>(m_distrElection(m_generator));
}

const GroupLead::NodeState GroupLead::GetNodeState() const {
    return m_currentState;
}

void GroupLead::SetOnLeaderChanged(std::function<void(const std::string &)> handler) {
    m_onLeaderChanged = handler;
}

void GroupLead::SendRequestForVote() {
    capnp::MallocMessageBuilder requestForVoteBuilder;
    auto msgInternals  = requestForVoteBuilder.initRoot<riaps::distrcoord::GroupInternals>();
    auto msgLeader     = msgInternals.initLeaderElection();
    auto msgReqForVote = msgLeader.initRequestForVoteReq();
    msgReqForVote.setSourceComponentId(GetComponentId());
    msgReqForVote.setElectionTerm(m_electionTerm);

    m_group->SendInternalMessage(requestForVoteBuilder);
}

void GroupLead::OnActionProposeFromClient(riaps::distrcoord::Consensus::ProposeToLeader::Reader &headerMessage,
                                          riaps::distrcoord::Consensus::TimeSyncCoordA::Reader  &tscaMessage) {
    if (GetLeaderId() != GetComponentId()) {
        //_logger->debug("OnProposeFromClient() returns, GetLeaderId() != GetComponentId()");
        return;
    }

    std::string actionId = tscaMessage.getActionId();

    /**
     * If the same action is being voted, and not timed out -> new vote is not started
     */
    if (m_actionData.find(actionId)!=m_actionData.end()){
        if (!m_actionData[actionId]->proposeDeadline.IsTimeout()) {
            _logger->debug("The previous voting process has not finished for action {},"
                           "new propose is not accepted until the previous vote is in progress", actionId);
            return;
        }
    }

    auto pd = std::shared_ptr<ProposeData>(new ProposeData(m_group->GetKnownComponents(), Timeout(duration<int, std::milli>(1000))));
    pd->isAction = true;
    pd->actionId = tscaMessage.getActionId();
    std::string proposeId = headerMessage.getProposeId();
    m_proposeData[proposeId] = pd;
    m_actionData [tscaMessage.getActionId().cStr()] = pd;

    capnp::MallocMessageBuilder builder;
    auto msgInternal = builder.initRoot<riaps::distrcoord::GroupInternals>();
    auto msgConsensus = msgInternal.initConsensus();
    auto msgPropose = msgConsensus.initProposeToClients();
    msgConsensus.setSourceComponentId(GetComponentId());
    msgConsensus.setVoteType(riaps::distrcoord::Consensus::VoteType::ACTION);
    msgPropose.setProposeId(proposeId);
    msgPropose.setLeaderId(GetLeaderId());
    auto msgTsynca = msgConsensus.initTsyncCoordA();
    msgTsynca.setActionId(actionId);
    auto msgTime = msgTsynca.initTime();
    msgTime.setTvSec(tscaMessage.getTime().getTvSec());
    msgTime.setTvNsec(tscaMessage.getTime().getTvNsec());



    zmsg_t* msg = zmsg_new();
    zframe_t* header;
    header << builder;
    zmsg_add(msg, header);


    if (m_group->SendMessage(&msg, INTERNAL_PUB_NAME))
        _logger->debug("GroupLead::OnActionProposeFromClient() - Message sent, proposeId: {} leaderId: {} sourceId: {} actionId: {}", proposeId, m_leaderId, GetComponentId(), actionId);
    else
        _logger->error("OnActionProposeFromClient() failed to send");
}

void GroupLead::OnProposeFromClient(riaps::distrcoord::Consensus::ProposeToLeader::Reader& headerMessage,
                                  zframe_t** messageFrame) {

    //_logger->debug("OnProposeFromClient()");
    if (GetLeaderId() != GetComponentId()) {
        //_logger->debug("OnProposeFromClient() returns, GetLeaderId() != GetComponentId()");
        return;
    }
    //_logger->debug("OnProposeFromClient() continues");


    auto pd = std::shared_ptr<ProposeData>(new ProposeData(m_group->GetKnownComponents(), Timeout(duration<int, std::milli>(1000))));

    // TODO: Add known node ids
    //pd.nodesInVote =
    std::string proposeId = headerMessage.getProposeId();
    m_proposeData[proposeId] = std::move(pd);

    // Send propose to clients
    capnp::MallocMessageBuilder builder;
    auto msgInternal = builder.initRoot<riaps::distrcoord::GroupInternals>();
    auto msgConsensus = msgInternal.initConsensus();
    auto msgPropose = msgConsensus.initProposeToClients();
    msgConsensus.setSourceComponentId(GetComponentId());
    msgConsensus.setVoteType(riaps::distrcoord::Consensus::VoteType::VALUE);
    msgPropose.setProposeId(proposeId);
    msgPropose.setLeaderId(GetLeaderId());

    zmsg_t* msg = zmsg_new();
    zframe_t* header;
    header << builder;
    zmsg_add(msg, header);
    zmsg_add(msg, *messageFrame);
    if (m_group->SendMessage(&msg, INTERNAL_PUB_NAME))
        _logger->debug("GroupLead::OnProposeFromClient() - Message sent, proposeId: {} leaderId: {} sourceId: {}", proposeId, m_leaderId, GetComponentId());
    else
        _logger->error("OnProposeFromClient() failed to send");
    *messageFrame = nullptr;
}

void GroupLead::OnVote(riaps::distrcoord::Consensus::Vote::Reader &message, const std::string& sourceComponentId) {
    std::string proposeId    = message.getProposeId();

    // May the leader changed
    //if (voteLeaderId != GetLeaderId()) return;

    // Shouldn't be true, but anyway just and an extra check
    //if (voteLeaderId != GetComponentId()) return;

    // If this proposeId is not registerd
    // Maybe the leader changed, or just already timed out and ereased.
    if (m_proposeData.find(proposeId) == m_proposeData.end()){
        _logger->debug("Propose Id was not found in the queue: {}", proposeId);
        return;
    }
    if (m_proposeData[proposeId]->proposeDeadline.IsTimeout()) {
        _logger->debug("Propose timeout, removed from the queue: {}", proposeId);

        // If it is an action, remove the action from the action queue
        if (m_proposeData[proposeId]->isAction &&
            m_actionData.find(m_proposeData[proposeId]->actionId) != m_proposeData.end()) {
            m_actionData.erase(m_proposeData[proposeId]->actionId);
        }

        m_proposeData.erase(proposeId);

        // No chance to accept
        Announce(proposeId, riaps::distrcoord::Consensus::VoteResults::REJECTED);
        return;
    }

    ProposeData* currentItem = m_proposeData[proposeId].get();
    auto nodesInVote         = currentItem->nodesInVote.get();
    auto nodesVoted          = currentItem->nodesVoted.get();

    // This node is not allowed to vote, was not in the group when the process started
    if (nodesInVote->find(sourceComponentId) == nodesInVote->end()) {
        return;
    }

    nodesVoted->insert(sourceComponentId);

    if (message.getVoteResult() == riaps::distrcoord::Consensus::VoteResults::ACCEPTED)
        currentItem->accepted++;
    else
        currentItem->rejected++;


    auto accepted  = currentItem->accepted;
    auto rejected  = currentItem->rejected;
    auto groupSize = nodesInVote->size();
    auto majority  = (groupSize%2==0)?(groupSize/2)+1:ceil(((double)groupSize)/2.0);

    // Majority, announce
    if (accepted >= majority) {
        Announce(proposeId, riaps::distrcoord::Consensus::VoteResults::ACCEPTED);
        if (m_proposeData[proposeId]->isAction &&
            m_actionData.find(m_proposeData[proposeId]->actionId) != m_proposeData.end()) {
            m_actionData.erase(m_proposeData[proposeId]->actionId);
        }
        m_proposeData.erase(proposeId);
        _logger->debug("Majority in VOTE => ANNOUNCE, {}/{}", groupSize, accepted);
    } else if(rejected>=majority){
        if (m_proposeData[proposeId]->isAction &&
            m_actionData.find(m_proposeData[proposeId]->actionId) != m_proposeData.end()) {
            m_actionData.erase(m_proposeData[proposeId]->actionId);
        }
        Announce(proposeId, riaps::distrcoord::Consensus::VoteResults::REJECTED);
        m_proposeData.erase(proposeId);
        _logger->debug("No majority in VOTE => ANNOUNCE, {}/{}", groupSize, accepted);
    } else {
        _logger->debug("No majority in VOTE, waiting for more vote {}/{}", groupSize, accepted);
    }
}

void GroupLead::Announce(const std::string& proposeId, riaps::distrcoord::Consensus::VoteResults result) {
    capnp::MallocMessageBuilder builder;
    auto msgInt       = builder.initRoot<riaps::distrcoord::GroupInternals>();
    auto msgConsensus = msgInt.initConsensus();
    auto msgAnnounce  = msgConsensus.initAnnounce();
    msgAnnounce.setProposeId(proposeId);
    msgAnnounce.setVoteResult(result);
    m_group->SendInternalMessage(builder);
}

void GroupLead::SendAppendEntry() {
    capnp::MallocMessageBuilder appendEntryBuilder;
    auto msgInternals  = appendEntryBuilder.initRoot<riaps::distrcoord::GroupInternals>();
    auto msgLeader     = msgInternals.initLeaderElection();
    auto msgAppendEntry = msgLeader.initAppendEntry();
    msgAppendEntry.setSourceComponentId(GetComponentId());
    msgAppendEntry.setElectionTerm(m_electionTerm);

    m_group->SendInternalMessage(appendEntryBuilder);
}

void GroupLead::SendVote(const std::string& voteFor) {
    capnp::MallocMessageBuilder voteBuilder;
    auto msgInternals = voteBuilder.initRoot<riaps::distrcoord::GroupInternals>();
    auto msgLeader = msgInternals.initLeaderElection();
    auto msgVote = msgLeader.initRequestForVoteRep();
    msgVote.setElectionTerm(m_electionTerm);
    msgVote.setSourceComponentId(GetComponentId());
    msgVote.setVoteForId(voteFor);

    m_group->SendInternalMessage(voteBuilder);
}

void GroupLead::ChangeLeader(const std::string &newLeader) {
    if (m_leaderId!=newLeader){
        m_leaderId = newLeader;
        if (m_onLeaderChanged){
            m_onLeaderChanged(newLeader);
        }
    }
}

GroupLead::~GroupLead() {

}

GroupLead::ProposeData::ProposeData(std::shared_ptr<std::set<std::string>> _knownNodes, Timeout &&timeout)
    : nodesInVote(_knownNodes), proposeDeadline(timeout), accepted(0), rejected(0){
    auto p = new std::set<std::string>();
    nodesVoted = std::move(std::shared_ptr<std::set<std::string>>(p));
}
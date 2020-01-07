//
// Created by istvan on 12/18/17.
//

#include <groups/r_grouplead.h>
#include <algorithm>
#include <framework/rfw_network_interfaces.h>


using namespace riaps::groups;
using namespace std::chrono;
using namespace std;

namespace gr = riaps::groups::data;

GroupLead::GroupLead(Group* group, std::unordered_map<OwnId, riaps::utils::Timeout<std::chrono::milliseconds>,
        OwnIdHasher,
        OwnIdComparator>* known_nodes)
    : group_(group),
      known_nodes_(known_nodes)
{

    /**
     * Initialize random generators
     */
    rnd_generator_        = std::mt19937(m_rd());
    election_distr_    = std::uniform_int_distribution<int>(GROUP_ELECTION_MIN, GROUP_ELECTION_MAX);
    election_timeout_  = Timeout<std::chrono::milliseconds>(GenerateElectionTimeo());
    appentry_timeout_  = Timeout<std::chrono::milliseconds>(AUTHORITY_TIMEOUT);
    election_term_     = 0;

    auto iface = riaps::framework::Network::GetConfiguredIface();
    auto host = riaps::framework::Network::GetIPAddress(iface);
    string pub_endpoint = fmt::format("tcp://{}:!",host);

    group_ansport_ = group->group_ansport();
    group_qryport_ = group->group_qryport();

    /**
     * In RAFT everybody starts in the FOLLOWER state
     */
    current_state_ = NodeState::FOLLOWER;

    logger_ = group->parent_component()->component_logger();
    leaderid_ = nullopt;
}

// TODO: use std::reference_wrapper to return a reference and do not copy the whole id.
std::optional<OwnId> GroupLead::GetLeaderId() {
    return leaderid_;
}

void GroupLead::Update() {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    /**
     * If the node is FOLLOWER and the wait time is not expired, then do nothing
     */
    if (current_state_ == GroupLead::NodeState::FOLLOWER && !election_timeout_.IsTimeout()){
        return;
    }

    /**
     * The node is a FOLLOWER but the wait time is expired without hearing from the leader
     * FOLLOWER --> CANDIDATE
     */
    else if (current_state_ == GroupLead::NodeState::FOLLOWER && election_timeout_.IsTimeout()) {

        ChangeLeader(nullopt);
        /**
         * Step into the next state and send REQUEST_VOTE message to everybody in the group
         * The number of nodes are saved since the MAJORITY of votes is needed
         */
        current_state_ = GroupLead::NodeState::CANDIDATE;

        /**
         * Note: +1 is added becuase the current node is not included in the GroupMemberCount();
         */
        number_of_nodes_in_vote_ = group_->GetMemberCount() + 1;

        /**
         * Set the first election Term
         */
        election_term_++;
        //m_logger->debug("[{}] Election timeout FOLLOWER->CANDIDATE", _electionTerm);

        /**
         * The node votes to itself, save the timestamp to filter possibly old votes
         */
        votes_.clear();
        votes_[GetOwnId()] = now;

        // TODO: What to do when only one node is in the group??? This is not trivial. RAFT doesn't work with one node.
        // But RIAPS should?
        logger_->debug("FOLLOWER, timeout => Candidate (id: {}, term: {})", GetOwnId().strdata(), election_term_);
        SendRequestForVote();

        /**
         * Waiting for responses until the MAX election timeout.
         */
        election_timeout_.Reset(std::chrono::milliseconds(GROUP_ELECTION_MAX));

    }

    /**
     * If the node is candidate and the wait time (vote time when candidate) has expired
     * It means that nobody responded to the vote request.
     */
    else if (current_state_ == GroupLead::CANDIDATE && !election_timeout_.IsTimeout()){
        logger_->debug("CANDIDATE, !timeout");
    } else if (current_state_ == GroupLead::CANDIDATE && election_timeout_.IsTimeout()){
        logger_->debug("CANDIDATE, timeout => FOLLOWER");
        // Vote is still in progress, but the timeout has expired.
        // Be a follower again, maybe better luck next time.

        //m_logger->debug("[{}] Vote timeout CANDIDATE->FOLLOWER",_electionTerm);

        current_state_ = GroupLead::NodeState::FOLLOWER;
        election_timeout_.Reset(std::chrono::milliseconds(GROUP_ELECTION_MAX));
    }

    /**
     * If no incoming message for the leader, then the leader just heartbeating
     * Note: The receivers don't respond to AppendEndtry (no log replication in RIAPS so far,
     * no reason to implement the reply)
     */
    else if (current_state_ == GroupLead::LEADER && appentry_timeout_.IsTimeout()) {
        SendAuthority();
        appentry_timeout_.Reset();
        leaderid_ = GetOwnId();
    }
    /**
     * If no incoming message for the leader and no timeout for sending heartbeat, then maintain the things.
     *  like: Checking ongoing vote timeouts
     */
    else if (current_state_ == GroupLead::LEADER && !appentry_timeout_.IsTimeout()) {

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

const OwnId& GroupLead::GetOwnId() const {
    return group_->own_id_;
}

void GroupLead::UpdateReqVote(const riaps::groups::data::ReqVote & reqvote) {
    /**
     * RequestForVote request arrived and the node is a follower.
     */
    if (current_state_ == GroupLead::NodeState::FOLLOWER) {
        logger_->debug("{} {}", __FUNCTION__, "Follower");
        /**
         * If the component didn't vote in this round (term) and the sender is a different component then VOTE
         */
        if (election_term_ < reqvote.term() && reqvote.ownid() != GetOwnId()){
            election_term_ = reqvote.term();
            election_timeout_.Reset(GenerateElectionTimeo());
            SendVote(election_term_, reqvote.ownid(), true);
        }
    }
    else if (current_state_ == GroupLead::NodeState::CANDIDATE || current_state_ == GroupLead::NodeState::LEADER){
        logger_->debug("{} {}", __FUNCTION__, "Candidate || Leader");
        // This is tricky. Looks like somebody else also started a vote.
        if (reqvote.term() >= election_term_){
            current_state_ = GroupLead::NodeState::FOLLOWER;
            election_term_ = reqvote.term();
            election_timeout_.Reset(GenerateElectionTimeo());
            SendVote(election_term_, reqvote.ownid(), true);
            logger_->debug("Vote: YES");
        } else {
            SendVote(election_term_, reqvote.ownid(), false);
            logger_->debug("Vote: NO");
        }
    }
}

//void GroupLead::Update(const char* command, zsock_t* socket) {
//    auto now = steady_clock::now();
//
//    //else if (internalMessage.hasRequestForVoteReq() && _currentState == GroupLead::NodeState::LEADER){
//        /**
//         * Do nothing. This guy is already a leader.
//         */
//    //}
//
//    /**
//     * RequestForVote response arrived
//     */
////    if (streq(command, RSPVOTE) && current_state_ == CANDIDATE) {
////        // term; candId; bool; ownId
////        auto term_frame    = zframe_recv(socket);
////        auto candid_frame  = zframe_recv(socket);
////        auto bool_frame    = zframe_recv(socket);
////        auto ownid_frame   = zframe_recv(socket);
////
////        auto elect_term = atoi((char*)zframe_data(term_frame));
////        auto voted_to    = std::string((char*)zframe_data(candid_frame));
////        auto vote_from  = std::string((char*)zframe_data(ownid_frame));
////
//////        std::string votedTo  = theVote.getVoteForId();
//////        std::string voteFrom = theVote.getSourceComponentId();
//////        auto        elecTerm = theVote.getElectionTerm();
////        //(*known_nodes_)[voteFrom] = zclock_mono();
////
////        /**
////         * Accept the vote only if the vote is for the current term and the From is not the current node
////         */
////        if (GetOwnId() != vote_from && elect_term == election_term_ && voted_to == GetOwnId()) {
////            votes_[vote_from] = steady_clock::now();
////
////            bool has_majority = false;
////            if (number_of_nodes_in_vote_ % 2 == 0){
////                uint32_t majority = (number_of_nodes_in_vote_ / 2) + 1;
////                auto number_of_votes = GetNumberOfVotes();
////                if (majority <= number_of_votes)
////                    has_majority = true;
////                else
////                    logger_->debug("No majority ({}): {}/{}", majority, number_of_nodes_in_vote_, number_of_votes);
////            } else if (number_of_nodes_in_vote_ % 2 == 1){
////                uint32_t majority = ceil(number_of_nodes_in_vote_ / 2.0);
////                auto number_of_votes = GetNumberOfVotes();
////                if (majority <= number_of_votes)
////                    has_majority = true;
////                else
////                    logger_->debug("No majority ({}): {}/{}", majority, number_of_nodes_in_vote_, number_of_votes);
////            }
////
////            /**
////             * MAJORITY ACHIEVED, send append entry, switch state
////             */
////            if (has_majority){
////                current_state_ = GroupLead::LEADER;
////                SendAppendEntry();
////                ChangeLeader(GetOwnId());
////            }
////        }
////    }
////    else if (internalMessage.hasAppendEntry() && current_state_ == GroupLead::FOLLOWER){
////        auto msgAppendEntry = internalMessage.getAppendEntry();
////        //(*known_nodes_)[msgAppendEntry.getSourceComponentId().cStr()] = zclock_mono();
////        //m_logger->debug("Append entry from: {0}", msgAppendEntry.getSourceComponentId().cStr());
////        m_electionTimeout.Reset(GenerateElectionTimeo());
////        election_term_ = msgAppendEntry.getElectionTerm();
////        ChangeLeader(msgAppendEntry.getSourceComponentId().cStr());
////    } else if (internalMessage.hasAppendEntry() && current_state_ == GroupLead::LEADER){
////        /**
////         * Something went wrong, cannot be two leader, the leader with less election term will be a follower
////         */
////        auto msgAppendEntry = internalMessage.getAppendEntry();
////        auto electTerm = msgAppendEntry.getElectionTerm();
////
////        if (electTerm > election_term_){
////            current_state_ = GroupLead::NodeState::FOLLOWER;
////            election_term_ = electTerm;
////            ChangeLeader(msgAppendEntry.getSourceComponentId());
////            m_electionTimeout.Reset(GenerateElectionTimeo());
////        }
////    }
//}

// TODO: check timestamps
uint32_t GroupLead::GetNumberOfVotes() {
    return votes_.size();
}

int64_t GroupLead::GenerateElectionTimeo() {
    return election_distr_(rnd_generator_);
}

const GroupLead::NodeState GroupLead::GetNodeState() const {
    return current_state_;
}

void GroupLead::SetOnLeaderChanged(std::function<void(const std::string &)> handler) {
    m_onLeaderChanged = handler;
}

void GroupLead::SendRequestForVote() {
    logger_->debug("{}", __FUNCTION__);
    zmsg_t* rfv_message = zmsg_new();
    zmsg_addstr(rfv_message, REQVOTE);
    riaps::groups::data::ReqVote reqvote(group_->own_id_, election_term_);
    reqvote.logger_ = logger_;
    vector<uint8_t> reqbytes = reqvote.to_bytes();

    zmsg_addmem(rfv_message, reqbytes.data(), reqbytes.size());
    group_->SendMessage(&rfv_message);
}

void GroupLead::SendAuthority() {
    zmsg_t* auth_message = zmsg_new();
    zmsg_addstr(auth_message, RSPVOTE);
    gr::Authority authority(election_term_, group_->own_id_, group_ansport_->host(), group_ansport_->port());
    auto auth_bytes = authority.to_bytes();
    zmsg_addmem(auth_message, auth_bytes.data(), auth_bytes.size());
    group_->SendMessage(&auth_message);
}

void GroupLead::OnActionProposeFromClient(riaps::distrcoord::Consensus::ProposeToLeader::Reader &headerMessage,
                                          riaps::distrcoord::Consensus::TimeSyncCoordA::Reader  &tscaMessage) {
    if (GetLeaderId() != GetOwnId()) {
        //m_logger->debug("OnProposeFromClient() returns, leader_id() != GetOwnId()");
        return;
    }

    std::string actionId = tscaMessage.getActionId();

    /**
     * If the same action is being voted, and not timed out -> new vote is not started
     */
    if (m_actionData.find(actionId)!=m_actionData.end()){
        if (!m_actionData[actionId]->proposeDeadline.IsTimeout()) {
            logger_->debug("The previous voting process has not finished for action {},"
                           "new propose is not accepted until the previous vote is in progress", actionId);
            return;
        }
    }

    auto pd = std::shared_ptr<ProposeData>(new ProposeData(group_->GetKnownComponents(), Timeout<std::chrono::milliseconds>(std::chrono::milliseconds(1000))));
    pd->isAction = true;
    pd->actionId = tscaMessage.getActionId();
    std::string proposeId = headerMessage.getProposeId();
    m_proposeData[proposeId] = pd;
    m_actionData [tscaMessage.getActionId().cStr()] = pd;

    capnp::MallocMessageBuilder builder;
    auto msgInternal = builder.initRoot<riaps::distrcoord::GroupInternals>();
    auto msgConsensus = msgInternal.initConsensus();
    auto msgPropose = msgConsensus.initProposeToClients();
    //msgConsensus.setSourceComponentId(GetOwnId());
    msgConsensus.setVoteType(riaps::distrcoord::Consensus::VoteType::ACTION);
    msgPropose.setProposeId(proposeId);
    //msgPropose.setLeaderId(GetLeaderId());
    auto msgTsynca = msgConsensus.initTsyncCoordA();
    msgTsynca.setActionId(actionId);
    auto msgTime = msgTsynca.initTime();
    msgTime.setTvSec(tscaMessage.getTime().getTvSec());
    msgTime.setTvNsec(tscaMessage.getTime().getTvNsec());



    zmsg_t* msg = zmsg_new();
    zframe_t* header;
    header << builder;
    zmsg_add(msg, header);


//    if (group_->SendMessage(&msg))
//        logger_->debug("GroupLead::OnActionProposeFromClient() - Message sent, proposeId: {} leader_id: {} sourceId: {} actionId: {}", proposeId, leaderid_,
//                       GetOwnId(), actionId);
//    else
//        logger_->error("OnActionProposeFromClient() failed to send");
}

void GroupLead::UpdateAuthority(const riaps::groups::data::Authority &authority) {
    // Authority arrived while this node is a candidate or a follower.
    // Means other node is elected as leader.
    // If the current node is a candidate then it is degraded to FOLLOWER
    if (current_state_ == GroupLead::NodeState::FOLLOWER || current_state_ == GroupLead::NodeState::CANDIDATE) {
        current_state_ = GroupLead::NodeState::FOLLOWER;
        if (election_term_ >= authority.term() && leaderid_ && leaderid_ != authority.ldrid()){
            logger_->error(
                    "Leader conflict in ",
                    group_->group_id().group_type_id,
                    group_->group_id().group_name);
        }
        election_term_ = authority.term();
        // Random timeout
        election_timeout_ = Timeout<std::chrono::milliseconds>(GenerateElectionTimeo());
        if (!leaderid_ || (leaderid_ && leaderid_ != authority.ldrid())) {
            if (leaderid_) {
                group_qryport_->Disconnect(leader_address_);
                logger_->debug("New leader: {} -> {}", leaderid_->strdata(), authority.ldrid().strdata());
            } else {
                logger_->debug("New leader: {}", authority.ldrid().strdata());
            }
            leader_address_ = authority.ldraddress_zmq();
            leaderid_ = authority.ldrid();
            group_qryport_->Connect(leader_address_);
        }
    } else {
        logger_->debug("Authority message arrived, but cannot be processed in the current state. Fn: {} Line: {}",
                __FUNCTION__, __LINE__);
    }
}

void GroupLead::UpdateRspVote(const riaps::groups::data::RspVote &rspvote) {
    // Count the vote only if the current node is a candidate
    if (current_state_ == GroupLead::NodeState::CANDIDATE) {
        /**
         * Accept the vote only if the vote is for the current term and the From is not the current node
         */
        if (rspvote.term() == election_term_ && rspvote.vote_for() == GetOwnId()) {
            votes_[rspvote.voted_by()] = steady_clock::now();

            bool has_majority = false;
            if (number_of_nodes_in_vote_ % 2 == 0){
                uint32_t majority = (number_of_nodes_in_vote_ / 2) + 1;
                auto number_of_votes = GetNumberOfVotes();
                if (majority <= number_of_votes)
                    has_majority = true;
                else
                    logger_->debug("No majority ({}): {}/{}", majority, number_of_nodes_in_vote_, number_of_votes);
            } else if (number_of_nodes_in_vote_ % 2 == 1){
                uint32_t majority = ceil(number_of_nodes_in_vote_ / 2.0);
                auto number_of_votes = GetNumberOfVotes();
                if (majority <= number_of_votes)
                    has_majority = true;
                else
                    logger_->debug("No majority ({}): {}/{}", majority, number_of_nodes_in_vote_, number_of_votes);
            }

            /**
             * MAJORITY ACHIEVED, send authority, switch state
             */
            if (has_majority){
                logger_->debug("Majority was achieved");
                current_state_ = GroupLead::LEADER;
                SendAuthority();
                ChangeLeader(GetOwnId());
            }
        }
    }
}

void GroupLead::OnProposeFromClient(riaps::distrcoord::Consensus::ProposeToLeader::Reader& headerMessage,
                                  zframe_t** messageFrame) {

    //m_logger->debug("OnProposeFromClient()");
    if (GetLeaderId() != GetOwnId()) {
        //m_logger->debug("OnProposeFromClient() returns, leader_id() != GetOwnId()");
        return;
    }
    //m_logger->debug("OnProposeFromClient() continues");


    auto pd = std::shared_ptr<ProposeData>(new ProposeData(group_->GetKnownComponents(), Timeout<std::chrono::milliseconds>(std::chrono::milliseconds(1000))));

    // TODO: Add known node ids
    //pd.nodesInVote =
    std::string proposeId = headerMessage.getProposeId();
    m_proposeData[proposeId] = std::move(pd);

    // Send propose to clients
//    capnp::MallocMessageBuilder builder;
//    auto msgInternal = builder.initRoot<riaps::distrcoord::GroupInternals>();
//    auto msgConsensus = msgInternal.initConsensus();
//    auto msgPropose = msgConsensus.initProposeToClients();
//    msgConsensus.setSourceComponentId(GetOwnId());
//    msgConsensus.setVoteType(riaps::distrcoord::Consensus::VoteType::VALUE);
//    msgPropose.setProposeId(proposeId);
//    msgPropose.setLeaderId(GetLeaderId());
//
//    zmsg_t* msg = zmsg_new();
//    zframe_t* header;
//    header << builder;
//    zmsg_add(msg, header);
//    zmsg_add(msg, *messageFrame);
//    if (group_->SendMessage(&msg))
//        logger_->debug("GroupLead::OnProposeFromClient() - Message sent, proposeId: {} leader_id: {} sourceId: {}", proposeId, leaderid_,
//                       GetOwnId());
//    else
//        logger_->error("OnProposeFromClient() failed to send");
//    *messageFrame = nullptr;
}

void GroupLead::OnVote(riaps::distrcoord::Consensus::Vote::Reader &message, const std::string& sourceComponentId) {
    std::string proposeId    = message.getProposeId();

    // May the leader changed
    //if (voteLeaderId != leader_id()) return;

    // Shouldn't be true, but anyway just and an extra check
    //if (voteLeaderId != GetOwnId()) return;

    // If this proposeId is not registerd
    // Maybe the leader changed, or just already timed out and ereased.
    if (m_proposeData.find(proposeId) == m_proposeData.end()){
        logger_->debug("Propose Id was not found in the queue: {}", proposeId);
        return;
    }
    if (m_proposeData[proposeId]->proposeDeadline.IsTimeout()) {
        logger_->debug("Propose timeout, removed from the queue: {}", proposeId);

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
        logger_->debug("Majority in VOTE => ANNOUNCE, {}/{}", groupSize, accepted);
    } else if(rejected>=majority){
        if (m_proposeData[proposeId]->isAction &&
            m_actionData.find(m_proposeData[proposeId]->actionId) != m_proposeData.end()) {
            m_actionData.erase(m_proposeData[proposeId]->actionId);
        }
        Announce(proposeId, riaps::distrcoord::Consensus::VoteResults::REJECTED);
        m_proposeData.erase(proposeId);
        logger_->debug("No majority in VOTE => ANNOUNCE, {}/{}", groupSize, accepted);
    } else {
        logger_->debug("No majority in VOTE, waiting for more vote {}/{}", groupSize, accepted);
    }
}

void GroupLead::Announce(const std::string& proposeId, riaps::distrcoord::Consensus::VoteResults result) {
    capnp::MallocMessageBuilder builder;
    auto msgInt       = builder.initRoot<riaps::distrcoord::GroupInternals>();
    auto msgConsensus = msgInt.initConsensus();
    auto msgAnnounce  = msgConsensus.initAnnounce();
    msgAnnounce.setProposeId(proposeId);
    msgAnnounce.setVoteResult(result);
    group_->SendInternalMessage(builder);
}

void GroupLead::SendVote(uint32_t term, const OwnId &vote_for, bool vote) {
    zmsg_t* vote_message = zmsg_new();
    zmsg_addstr(vote_message, RSPVOTE);
    gr::RspVote rspvotes(term, vote_for, group_->own_id_, vote);
    auto rsp_bytes = rspvotes.to_bytes();
    zmsg_addmem(vote_message, rsp_bytes.data(), 40);
    group_->SendMessage(&vote_message);
}

void GroupLead::ChangeLeader(optional<OwnId> new_leader) {
    if (leaderid_ != new_leader){
        leaderid_ = new_leader;
//        if (m_onLeaderChanged){
//            m_onLeaderChanged(newLeader);
//        }
    }
}

GroupLead::~GroupLead() {

}

GroupLead::ProposeData::ProposeData(std::shared_ptr<std::set<std::string>> _knownNodes, Timeout<std::chrono::milliseconds> &&timeout)
    : nodesInVote(_knownNodes), proposeDeadline(timeout), accepted(0), rejected(0){
    auto p = new std::set<std::string>();
    nodesVoted = std::move(std::shared_ptr<std::set<std::string>>(p));
}
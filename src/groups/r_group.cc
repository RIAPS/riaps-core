//
// Created by istvan on 10/3/17.
//

#include <groups/r_group.h>
#include <capnp/common.h>
#include <groups/r_groupdata.h>

using namespace std;
using namespace riaps::discovery;

namespace  gr = riaps::groups::data;

namespace riaps{

    class ComponentBase;

    namespace groups{

        using namespace riaps;

        std::optional<OwnId> Group::leader_id() const {
                return group_leader_->GetLeaderId();
        }

        const groups::GroupId& Group::group_id() const {
            return group_id_;
        }

        Group::Group(const GroupId &group_id, ComponentBase* parentComponent) :
                group_id_(group_id),
                parent_component_(parentComponent),
                group_pubport_(nullptr),
                group_subport_(nullptr),
                group_leader_(nullptr),
                last_heartbeat_(0.0),
                group_read_buffer_(nullptr) {
            logger_ = parentComponent->component_logger();
            last_heartbeat_ = Timeout<std::chrono::milliseconds>(GROUP_HEARTBEAT);
            random_generator_ = std::mt19937(random_device_());
            timeout_distribution_ = std::uniform_int_distribution<int>(GROUP_HEARTBEAT*1.1, GROUP_HEARTBEAT*2);

            own_id_.data(parentComponent->actor()->actor_id(), (uint64_t)this);
            logger_->debug("ActorId: {} Pid: {}", parent_component()->actor()->actor_id_str(), getpid());
            logger_->debug("OwnId: {}", own_id_.strdata());
        }

        bool Group::InitGroup() {
            // If the groupid doesn't exist, just skip the initialization and return false
            auto group_conf = parent_component_->actor()->GetGroupType(group_id_.group_type_id);
            if (group_conf == nullptr) {
                logger_->error("Didn't find group {} in {}", group_id_.group_type_id, __FUNCTION__);
                return false;
            }

            group_conf_ = group_conf;

            // Default configuration of the group publisher port.
            GroupPortPub internalPubConfig;
            internalPubConfig.message_type = group_conf->message();
            internalPubConfig.is_local     = false;
            internalPubConfig.port_name    = INTERNAL_PUB_NAME;

            group_pubport_ = make_shared<ports::GroupPublisherPort>(internalPubConfig, parent_component_);

            // Default configuration of the group subscriber port.
            GroupPortSub internalSubConfig;
            internalSubConfig.message_type = group_conf->message();
            internalSubConfig.is_local     = false;
            internalSubConfig.port_name    = INTERNAL_SUB_NAME;

            group_subport_ = make_shared<ports::GroupSubscriberPort>(internalSubConfig, parent_component_, group_id_);
            group_subport_->Init();

            // Default configuration of the answer port for the leader.
            ComponentPortAns internal_ans_conf;
            internal_ans_conf.qry_type     = INTERNAL_ANS_REQ_TYPE;
            internal_ans_conf.ans_type     = INTERNAL_ANS_REP_TYPE;
            internal_ans_conf.is_local     = false;
            internal_ans_conf.message_type = fmt::format("{}#{}", INTERNAL_ANS_REQ_TYPE, INTERNAL_ANS_REP_TYPE);
            internal_ans_conf.port_name    = INTERNAL_ANS_NAME;

            group_ansport_ = make_shared<ports::AnswerPort>(internal_ans_conf, parent_component_);
            group_ansport_->Init();

            // Default configuration of the query port for the leader.
            ComponentPortQry internal_qry_conf;
            internal_ans_conf.qry_type     = INTERNAL_QRY_REQ_TYPE;
            internal_ans_conf.ans_type     = INTERNAL_QRY_REP_TYPE;
            internal_qry_conf.is_local     = false;
            internal_qry_conf.message_type = fmt::format("{}#{}", INTERNAL_QRY_REQ_TYPE, INTERNAL_QRY_REP_TYPE);
            internal_qry_conf.port_name    = INTERNAL_QRY_NAME;

            group_qryport_ = make_shared<ports::QueryPort>(internal_qry_conf, parent_component_);

            auto msg_type = fmt::format("{}@{}.{}", internalPubConfig.message_type, group_id_.group_type_id, group_id_.group_name);
            auto has_joined = Disco::RegisterService(parent_component_->actor()->application_name(),
                                           parent_component_->actor()->actor_name(),
                                           msg_type,
                                           group_pubport_->GetGroupService().host,
                                           group_pubport_->GetGroupService().port,
                                           riaps::discovery::Kind::GPUB,
                                           riaps::discovery::Scope::GLOBAL);

            // Setup leader election
            //if (has_joined && group_conf_->has_leader()) {
            group_leader_ = make_unique<riaps::groups::GroupLead>(this, &known_nodes_);

//                group_leader_->SetOnLeaderChanged([this](const std::string& newLeader){
//                    logger_->debug("Leader changed: {}", newLeader);
//                });
            //}

            return has_joined;
        }

//        bool Group::SendInternalMessage(capnp::MallocMessageBuilder &message) {
//            return SendMessage(message, INTERNAL_PUB_NAME);
//        }

        std::shared_ptr<riaps::ports::QueryPort> Group::group_qryport() {
            return group_qryport_;
        }

        std::shared_ptr<riaps::ports::AnswerPort> Group::group_ansport() {
            return group_ansport_;
        }

//        bool Group::ProposeValueToLeader(capnp::MallocMessageBuilder &message, const std::string &proposeId) {
//            bool hasActiveLeader = false; //leader_id() != "";
//            if (!hasActiveLeader){
//                logger_->error("ProposeValueToLeader(), no active leader, send failed");
//                return false;
//            }
//
//            zframe_t* frame;
//            frame << message;
//
//            capnp::MallocMessageBuilder builder;
//            auto msgGroupInternals = builder.initRoot<riaps::distrcoord::GroupInternals>();
//            auto msgCons = msgGroupInternals.initConsensus();
//            msgCons.setVoteType(riaps::distrcoord::Consensus::VoteType::VALUE);
//
//            auto msgPropLeader = msgCons.initProposeToLeader();
//            msgPropLeader.setProposeId(proposeId);
//            msgCons.setSourceComponentId(parent_component_id());
//
//            zframe_t* header;
//            header << builder;
//
//            zmsg_t* zmsg = zmsg_new();
//            zmsg_add(zmsg, header);
//            zmsg_add(zmsg, frame);
//
//            bool rc = SendMessage(&zmsg);
////            if (!rc)
////                logger_->error("ProposeValueToLeader() failed");
////            else
////                logger_->debug("ProposeValueToLeader() proposeId: {}, leader_id: {}, srcComp: {}", proposeId,
////                               leader_id(),
////                               parent_component_id());
//            return rc;
//        }
//
//        bool Group::ProposeActionToLeader(const std::string& proposeId,
//                                          const std::string &actionId,
//                                          const timespec &absTime) {
//            bool hasActiveLeader = false; //leader_id() != "";
//            if (!hasActiveLeader){
//                logger_->error("ProposeActionToLeader(), no active leader, send failed");
//                return false;
//            }
//
//
//            capnp::MallocMessageBuilder builder;
//            auto msgGroupInternals = builder.initRoot<riaps::distrcoord::GroupInternals>();
//            auto msgCons = msgGroupInternals.initConsensus();
//            auto msgPropLeader = msgCons.initProposeToLeader();
//
//            msgCons.setVoteType(riaps::distrcoord::Consensus::VoteType::ACTION);
//
//            msgPropLeader.setProposeId(proposeId);
//            msgCons.setSourceComponentId(parent_component_id());
//
//            auto msgTsyncA = msgCons.initTsyncCoordA();
//            msgTsyncA.setActionId(actionId);
//
//            auto msgTime = msgTsyncA.initTime();
//            msgTime.setTvSec(absTime.tv_sec);
//            msgTime.setTvNsec(absTime.tv_nsec);
//
//            zframe_t* header;
//            header << builder;
//
//            zmsg_t* zmsg = zmsg_new();
//            zmsg_add(zmsg, header);
//
//            bool rc = SendMessage(&zmsg);
//
////            if (!rc)
////                logger_->error("ProposeActionToLeader() failed");
////            else
////                logger_->debug("ProposeActionToLeader() proposeId: {}, leader_id: {}, srcComp: {}", proposeId,
////                              leader_id(),
////                              parent_component()->ComponentUuid());
//            return rc;
//        }

        std::shared_ptr<riaps::ports::GroupPublisherPort> Group::group_pubport() {
            return group_pubport_;
        }

        std::shared_ptr<riaps::ports::GroupSubscriberPort> Group::group_subport() {
            return group_subport_;
        }

        bool Group::HasLeader() {
            if (group_conf_->has_leader() && group_leader_!= nullptr) {
                return leader_id().has_value();
            }
        }

//        bool Group::SendMessage(capnp::MallocMessageBuilder& message, const std::string& portName){
//
//            // If the port is not specified, send on the internal port
//            if (portName != "") {
//                return group_pubport()->Send(message);
//            } else {
//                capnp::MallocMessageBuilder builder;
//                auto intMessage = builder.initRoot<riaps::distrcoord::GroupInternals>();
//                auto grpMessage = intMessage.initGroupMessage();
//                grpMessage.setSourceComponentId(parent_component_id());
//
//                zframe_t* header;
//                zframe_t* content;
//
//                header << builder;
//                content << message;
//
//                // append() takes the ownership, zframe_t*, zmsg_t* will be destroyed after send();
//                zmsg_t* zmsg = zmsg_new();
//                zmsg_append(zmsg, &header);
//                zmsg_append(zmsg, &content);
//                return SendMessage(&zmsg);
//            }
//            return false;
//        }

        bool Group::SendMessage(zmsg_t** message){
            logger_->debug("{}", __FUNCTION__);
            group_pubport()->Send(message);
        }

        std::shared_ptr<std::unordered_set<OwnId, OwnIdHasher>> Group::GetKnownMembers() {
            auto result = make_shared<std::unordered_set<OwnId, OwnIdHasher>>();

            for (auto& n : known_nodes_){
                if (n.second.IsTimeout()) continue;
                result->insert(n.first);
            }
            return result;
        }

        void Group::ConnectToNewServices(const std::string &address) {
            logger_->debug("{}::{} is connecting to {}", group_id().group_type_id, group_id().group_name, address);
            group_subport_->ConnectToPublihser(address);
        }

        const ComponentBase* Group::parent_component() const {
            return parent_component_;
        }

        const std::string Group::parent_component_id() const {
            return parent_component()->ComponentUuid();
        }

        uint16_t Group::GetMemberCount() {
            DeleteTimeoutNodes();
            return known_nodes_.size();
        }

        uint32_t Group::DeleteTimeoutNodes() {
            uint32_t deleted=0;
            for(auto it = std::begin(known_nodes_); it != std::end(known_nodes_);)
            {
                if ((*it).second.IsTimeout())
                {
                    it = known_nodes_.erase(it);
                    deleted++;
                }
                else
                    ++it;
            }
            return deleted;
        }

//        ports::GroupSubscriberPort* Group::FetchNextMessage(std::shared_ptr<capnp::FlatArrayMessageReader>& messageReader) {
//
//            void* which = zpoller_wait(group_poller_, 1);
//            if (which == nullptr){
//                // No incoming message, update the leader
//                if (_groupTypeConf.has_leader) {
//                    group_leader_->Update();
//                }
//                return nullptr;
//            }
//
//            // Look for the port
//            ports::PortBase* currentPort = nullptr;
//            try{
//                currentPort = group_ports_.at(static_cast<zsock_t*>(which)).get();
//            } catch (std::out_of_range& e){
//                return nullptr;
//            }
//
//            riaps::ports::GroupSubscriberPort* subscriberPort = currentPort->AsGroupSubscriberPort();
//
//            // If the port is not a subscriber port (in theory this is impossible)
//            if (subscriberPort == nullptr)  return nullptr;
//
//            // Delete the previous message if there is any
//            if (_lastFrame != nullptr) {
//                zframe_destroy(&_lastFrame);
//                _lastFrame = nullptr;
//            };
//
//            zmsg_t* msg = zmsg_recv(const_cast<zsock_t*>(subscriberPort->GetSocket()));
//
//            if (msg) {
//
//                // _lstFrame owns the data. Must be preserved until the next message
//                _lastFrame  = zmsg_pop(msg);
//                size_t size = zframe_size(_lastFrame);
//                byte *data  = zframe_data(_lastFrame);
//
//                kj::ArrayPtr<const capnp::word> capnp_data(reinterpret_cast<const capnp::word *>(data), size / sizeof(capnp::word));
//
//
//                messageReader.reset(new capnp::FlatArrayMessageReader(capnp_data));
//
//                // Internal port, handle it here and don't send any notifications to the caller
//                if (subscriberPort == _groupSubPort.get()){
//                    capnp::FlatArrayMessageReader reader(capnp_data);
//
//                    auto internal = reader.getRoot<riaps::distrcoord::GroupInternals>();
//                    if (internal.hasGroupMessage()) {
//                        // Read the content
//                        zframe_t* messageFrame;
//                        messageFrame = zmsg_pop(msg);
//                        if (messageFrame) {
//                            auto capnpReader = std::shared_ptr<capnp::FlatArrayMessageReader>(new capnp::FlatArrayMessageReader(nullptr));
//                            (*messageFrame) >> capnpReader;
//                            messageReader = capnpReader;
//                            //parent_component_->OnGroupMessage(_groupId, *capnpReader.get(), nullptr);
//                        }
//                    }
//                    else if (internal.hasGroupHeartBeat()) {
//                        auto groupHeartBeat = internal.getGroupHeartBeat();
//                        auto it = known_nodes_.find(groupHeartBeat.getSourceComponentId());
//
//                        // New node, set the timeout
//                        if (it == known_nodes_.end()) {
//                            known_nodes_[groupHeartBeat.getSourceComponentId().cStr()] =
//                                    Timeout<std::milli>(duration<int, std::milli>(timeout_distribution_(random_generator_)));
//                        } else
//                            it->second.Reset(duration<int, std::milli>(timeout_distribution_(random_generator_)));
//
//                        if (groupHeartBeat.getHeartBeatType() == riaps::distrcoord::HeartBeatType::PING) {
//                            //logger_->debug("<<PING<<");
//                            SendPong();
//                            return nullptr;
//                        } else if (groupHeartBeat.getHeartBeatType() == riaps::distrcoord::HeartBeatType::PONG) {
//                            //logger_->debug("<<PONG<<");
//                            return nullptr;
//                        }
//                    } else if (internal.hasLeaderElection()){
//                        auto msgLeader = internal.getLeaderElection();
//                        group_leader_->Update(msgLeader);
//                        return nullptr;
//                    } else if (internal.hasMessageToLeader()){
//                        auto msgLeader = internal.getMessageToLeader();
//                        if (leader_id() != parent_component()->GetCompUuid()) return nullptr;
//                        logger_->debug("Message to the leader arrived!");
//
//                        return nullptr;
//                    } else if (internal.has_consensus()) {
//                        auto msgCons = internal.getConsensus();
//                       // logger_->debug("DC message arrived from {}", msgDistCoord.getSourceComponentId().cStr());
//
////                        // The current component is the leader
//                        if (leader_id() == parent_component_id()) {
//                            //logger_->debug("DC message arrived and this component is the leader");
//
////                            // Propose arrived to the leader. Leader forwards it to every groupmember.
//                            // TODO: We may not need the forwarding step, since everybody got the message
//                            if (msgCons.hasProposeToLeader()){
//                                auto msgPropose = msgCons.getProposeToLeader();
//
//                                // Value is proposed
//                                if (msgCons.getVoteType() == riaps::distrcoord::Consensus::VoteType::VALUE){
//                                    zframe_t* proposeFrame;
//                                    proposeFrame = zmsg_pop(msg);
//                                    logger_->info("Message proposed to the leader, proposeId: {}", msgPropose.getProposeId().cStr());
//                                    group_leader_->OnProposeFromClient(msgPropose, &proposeFrame);
//                                    if (proposeFrame!= nullptr)
//                                        zframe_destroy(&proposeFrame);
//                                }
//
//                                // Action is proposed to the leader
//                                else if (msgCons.getVoteType() == riaps::distrcoord::Consensus::VoteType::ACTION) {
//                                    auto msgTsca = msgCons.getTsyncCoordA();
//                                    logger_->info("Action proposed to the leader, proposeId: {}, actionId: {}",
//                                                  msgPropose.getProposeId().cStr(),
//                                                  msgTsca.getActionId().cStr());
//                                    group_leader_->OnActionProposeFromClient(msgPropose,msgTsca);
//                                }
//
////                            // Vote arrived, count the votes and announce the results (if any)
//                            } else if (msgCons.hasVote()){
//                                //logger_->info("Vote arrived to the leader");
//                                auto msgVote = msgCons.getVote();
//                                group_leader_->OnVote(msgVote, msgCons.getSourceComponentId());
//                            }
//                        }
////                        // The current component is not a leader
//                        else {
//                            //logger_->debug("DC message arrived and this component is not the leader");
////                            // propose by the leader, must vote on something
//                            if (msgCons.hasProposeToClients()) {
//                                auto msgPropose = msgCons.getProposeToClients();
//                                //logger_->debug("Message proposed to the client, proposeId: {}", msgPropose.getProposeId().cStr());
//
//                                if (msgCons.getVoteType() == riaps::distrcoord::Consensus::VoteType::VALUE) {
//                                    zframe_t *proposeFrame;
//                                    proposeFrame = zmsg_pop(msg);
//                                    capnp::FlatArrayMessageReader *reader;
//                                    (*proposeFrame) >> reader;
//                                    parent_component_->OnPropose(_groupId, msgPropose.getProposeId(), *reader);
//                                } else if (msgCons.getVoteType() == riaps::distrcoord::Consensus::VoteType::ACTION) {
//                                    timespec t{
//                                            msgCons.getTsyncCoordA().getTime().getTvSec(),
//                                            msgCons.getTsyncCoordA().getTime().getTvNsec()
//                                    };
//
//                                    logger_->error_if(t.tv_sec!=msgCons.getTsyncCoordA().getTime().getTvSec(),"tv_sec!=TvSec");
//                                    logger_->error_if(t.tv_nsec!=msgCons.getTsyncCoordA().getTime().getTvNsec(),"tv_nsec!=TvNsec");
//
//                                    parent_component_->OnActionPropose(_groupId,
//                                                                      msgPropose.getProposeId(),
//                                                                      msgCons.getTsyncCoordA().getActionId(),
//                                                                      t);
//                                }
//                            } else if (msgCons.hasAnnounce()) {
//                                auto msgAnnounce = msgCons.getAnnounce();
//                                parent_component_->OnAnnounce(_groupId,
//                                                             msgAnnounce.getProposeId(),
//                                                             msgAnnounce.getVoteResult() == riaps::distrcoord::Consensus::VoteResults::ACCEPTED);
//                            }
//                        }
//
//                        return nullptr;
//                    }
//                }
//
//
//            }
//
//            zmsg_destroy(&msg);
//            return subscriberPort;
//        }

        double Group::GetPythonTime(timespec& now) {
            double sec = now.tv_sec;
            double rem = now.tv_nsec * 0.000000001;
            return sec+rem;
        }

        double Group::GetPythonNow() {
            timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            return GetPythonTime(ts);
        }

        std::string DoubleToStr(double val) {
            std::ostringstream strs;
            strs << val;
            return strs.str();
        }

        void Group::Heartbeat() {
            if (last_heartbeat_.IsTimeout()) {
                //logger_->debug("{}", __FUNCTION__);
                zmsg_t* msg_heartbeat = zmsg_new();
                zmsg_addstr(msg_heartbeat, HEARTBEAT);
                zmsg_addmem(msg_heartbeat, own_id_.data().data(), 16);
                group_pubport_->Send(&msg_heartbeat);
                last_heartbeat_.Reset();
            }
        }

        void Group::ProcessOnAns() {
            auto ans_socket  = const_cast<zsock_t*>(group_ansport_->port_socket());
            auto identity_str = zstr_recv(ans_socket);
            auto empty_delimeter = zframe_recv(ans_socket);
            auto msgtype_frame = zframe_recv(ans_socket);
            if (zframe_streq (msgtype_frame, GROUP_MTL)){
                pending_mtl_ = true;
                parent_component_->HandleMessageToLeader(this, identity_str);
                pending_mtl_ = false;
            } else {
                logger_->error("Unknown message type on group ANS port");
            }
            zstr_free(&identity_str);
            zframe_destroy(&empty_delimeter);
            zframe_destroy(&msgtype_frame);
        }

        void Group::ProcessOnQry() {
            auto qry_socket  = const_cast<zsock_t*>(group_qryport_->port_socket());
            auto first_frame = zframe_recv(qry_socket);
            if (zframe_streq (first_frame, GROUP_MFL)){
                pending_mfl_ = true;
                parent_component_->HandleMessageFromLeader(this);
                pending_mfl_ = false;
            } else {
                logger_->error("Unknown message type on group QRY port");
            }
            zframe_destroy(&first_frame);
        }

        void Group::ProcessOnSub() {
            auto sub_socket  = const_cast<zsock_t*>(group_subport_->port_socket());
            auto first_frame = zframe_recv(sub_socket);
            if (zframe_streq (first_frame, HEARTBEAT)){
                // Recv() the second frame of the message.
                // Must be the own_id of the another group.
                //auto peer = zstr_recv(sub_socket);
                auto second_frame = zframe_recv(sub_socket);
                auto second_size = zframe_size(second_frame);
                auto second_data = zframe_data(second_frame);
                OwnId ownid;
                ownid.data(second_data, second_size);
                known_nodes_[ownid] = Timeout<std::chrono::milliseconds>(GROUP_PEERTIMEOUT);
                zframe_destroy(&second_frame);
            } else if (zframe_streq (first_frame, GROUP_MSG)) {
                parent_component_->HandleGroupMessage(this);
            } else if (zframe_streq (first_frame, REQVOTE)) {
                auto req_frame = zframe_recv(sub_socket);
                auto req_data  = zframe_data(req_frame);
                gr::ReqVote reqvote(req_data);

                //logger_->debug("REQVOTE, string term: {} node: {}", reqvote.term(), reqvote.ownid().strdata());

                if (group_leader_ == nullptr)
                    logger_->error("Group leader is NULL");
                else
                    group_leader_->UpdateReqVote(reqvote);
                zframe_destroy(&req_frame);
            } else if (zframe_streq (first_frame, RSPVOTE)) {
                auto rsp_frame = zframe_recv(sub_socket);
                auto rsp_data  = zframe_data(rsp_frame);
                gr::RspVote rspvote(rsp_data);

                //logger_->debug("RSPVOTE term: {} vote for: {}", rspvote.term(), rspvote.vote_for().strdata());

                if (group_leader_ == nullptr)
                    logger_->error("Group leader is NULL");
                else
                    group_leader_->UpdateRspVote(rspvote);
                zframe_destroy(&rsp_frame);
            } else if (zframe_streq (first_frame, AUTHORITY)) {
                auto auth_frame = zframe_recv(sub_socket);
                auto auth_data  = zframe_data(auth_frame);
                gr::Authority auth(auth_data);
                //logger_->debug("Auth, term: {}, ldrid: {}, addr: {}", auth.term(), auth.ldrid().strdata(), auth.ldraddress_zmq());

                if (group_leader_ == nullptr)
                    logger_->error("Group leader is NULL");
                else
                    group_leader_->UpdateAuthority(auth);
                zframe_destroy(&auth_frame);
            } else if (zframe_streq (first_frame, GROUP_RCM)) {
                auto rcm_frame = zframe_recv(sub_socket);
                capnp::FlatArrayMessageReader rcm_reader(nullptr);
                (*rcm_frame)>>rcm_reader;
                auto gvote = rcm_reader.getRoot<riaps::groups::poll::GroupVote>();
                if (gvote.isRfv()) {
                    auto rfv = gvote.getRfv();

                    auto rfvid = rfv.getRfvId();
                    auto subject = rfv.getSubject();

                    if (subject == riaps::groups::poll::Subject::VALUE) {
                        auto topic = rfv.getTopic();
                        group_read_buffer_ = make_unique<MessageReaderArray>(topic.begin(), topic.size());
                        pending_handle_vote_request_ = true;
                        parent_component_->HandleVoteRequest(this, rfvid);
                        pending_handle_vote_request_ = false;
                    } else if (subject == riaps::groups::poll::Subject::ACTION) {
                        pending_handle_vote_request_ = true;
                        auto when = rfv.getRelease();
                        parent_component_->HandleActionVoteRequest(this,rfvid,when);
                        pending_handle_vote_request_ = false;
                    }

                } else {
                    logger_->error("First frame is {}, but the capnp message is different", GROUP_RCM);
                }
                zframe_destroy(&rcm_frame);

            } else if (zframe_streq (first_frame, GROUP_ANN)) {
                auto ann_frame = zframe_recv(sub_socket);
                capnp::FlatArrayMessageReader ann_reader(nullptr);
                (*ann_frame)>>ann_reader;
                auto gvote = ann_reader.getRoot<riaps::groups::poll::GroupVote>();
                if (gvote.isAnn()) {
                    auto ann = gvote.getAnn();
                    string rfvid = ann.getRfvId();
                    bool vote = ann.getVote() == riaps::groups::poll::Vote::YES?true:false;
                    parent_component_->HandleVoteResult(this, rfvid, vote);
                } else {
                    logger_->error("First frame is {}, but the capnp message is different", GROUP_ANN);
                }
            } else {
                // The second frame as string. Doesn't own the data. The data is destroyed when the frame is destroyed.
                char* frame_content = (char*)zframe_data(first_frame);
                logger_->debug("Unhandled messagetype in groups: {}", frame_content);
            }
            zframe_destroy(&first_frame);
        }

        std::optional<std::string> Group::RequestActionVote(const std::string &action, const double when, riaps::groups::poll::Voting kind, double timeout) {
            capnp::MallocMessageBuilder msg_builder;
            auto msg = msg_builder.initRoot<riaps::groups::poll::GroupVote>();
            auto rfv = msg.initRfv();
            rfv.setKind(kind);
            auto uid = zuuid_new();
            std::string uid_str = zuuid_str(uid);
            rfv.setRfvId(uid_str);
            rfv.setStarted(this->GetPythonNow());
            rfv.setSubject(riaps::groups::poll::Subject::ACTION);
            rfv.setTimeout(timeout/1000.0);
            rfv.setRelease(when);

            kj::StringPtr sptr(action);
            rfv.initTopic(action.size());
            rfv.setTopic(sptr.asBytes());

            zframe_t* message_frame;
            message_frame << msg_builder;
            zmsg_t* vote_message = zmsg_new();
            zmsg_addstr(vote_message, GROUP_RFV);
            zmsg_add(vote_message, message_frame);

            auto rc = group_qryport_->Send(&vote_message);

            zuuid_destroy(&uid);

            // Error
            if (rc) {
                return std::nullopt;
            }

            return uid_str;
        }

        void Group::SendVote(const std::string &rfvid, bool vote) {
            capnp::MallocMessageBuilder msg_builder;
            auto msg = msg_builder.initRoot<riaps::groups::poll::GroupVote>();
            auto rtc = msg.initRtc();
            rtc.setRfvId(rfvid);
            rtc.setVote(vote?riaps::groups::poll::Vote::YES:riaps::groups::poll::Vote::NO);

            zframe_t* message_frame;
            message_frame << msg_builder;
            zmsg_t* group_message = zmsg_new();
            zmsg_addstr(group_message, GROUP_RTC);
            zmsg_add(group_message, message_frame);
            this->group_qryport_->Send(&group_message);
        }

        void Group::FetchNextMessage(zsock_t* in_socket) {
            // If heartbeat timeout, send heartbeat.
            Heartbeat();

            // No incoming message, update the leader
            if (in_socket == nullptr) {
                if (group_conf_->has_leader()) {
                    group_leader_->Update();
                }
                return;
            }
            auto now = GetPythonNow();
            auto sub_socket  = const_cast<zsock_t*>(group_subport_->port_socket());
            auto qry_socket  = const_cast<zsock_t*>(group_qryport_->port_socket());
            auto ans_socket  = const_cast<zsock_t*>(group_ansport_->port_socket());

            if (in_socket == sub_socket) {
                ProcessOnSub();
            } else if (in_socket == qry_socket) {
                ProcessOnQry();
            } else if (in_socket == ans_socket) {
                ProcessOnAns();
            }
        }

//        bool Group::SendVote(const std::string &propose_id, bool accept) {
//            capnp::MallocMessageBuilder builder;
//            auto msgInt = builder.initRoot<riaps::distrcoord::GroupInternals>();
//            auto msgCons = msgInt.initConsensus();
//            msgCons.setSourceComponentId(parent_component()->ComponentUuid());
//            auto msgVote = msgCons.initVote();
//            msgVote.setProposeId(propose_id);
//            if (accept)
//                msgVote.setVoteResult(riaps::distrcoord::Consensus::VoteResults::ACCEPTED);
//            else
//                msgVote.setVoteResult(riaps::distrcoord::Consensus::VoteResults::REJECTED);
//
//            return SendInternalMessage(builder);
//        }

        Group::~Group() {
//            if (_lastFrame!= nullptr){
//                zframe_destroy(&_lastFrame);
//                _lastFrame=nullptr;
//            }
//            if (group_poller_ != nullptr)
//                zpoller_destroy(&group_poller_);
        }
    }
}
//
// Created by istvan on 10/3/17.
//

#include <groups/r_group.h>
#include <messaging/distcoord.capnp.h>
#include <capnp/common.h>

/*10s in msec*/
constexpr auto PING_BASE_PERIOD = 10*1000;

namespace dc = riaps::distrcoord;

using namespace std;
using namespace riaps::discovery;

namespace riaps{

    class ComponentBase;

    namespace groups{

        using namespace riaps;

        bool GroupId::operator<(const GroupId &other) const {
            if (group_type_id == other.group_type_id){
                return group_name < other.group_name;
            }
            return group_type_id<other.group_type_id;
        }

        bool GroupId::operator==(const GroupId &other) const {
            return group_type_id == other.group_type_id && group_name == other.group_name;
        }

        std::string Group::leader_id() const {
            if (group_type_conf_.has_leader && group_leader_ != nullptr){
                return group_leader_->GetLeaderId();
            }
            return "";
        }

        Group::Group(const GroupId &group_id, ComponentBase* parentComponent) :
                group_id_(group_id),
                parent_component_(parentComponent),
                group_pubport_(nullptr),
                group_subport_(nullptr),
                //_lastFrame(nullptr),
                group_leader_(nullptr),
                group_poller_(nullptr) {
            logger_ = spd::get(parentComponent->component_config().component_name);
            ping_timeout_ = Timeout<std::chrono::milliseconds>(PING_BASE_PERIOD);

            random_generator_ = std::mt19937(random_device_());
            timeout_distribution_ = std::uniform_int_distribution<int>(PING_BASE_PERIOD*1.1, PING_BASE_PERIOD*2);
        }

        bool Group::InitGroup() {

            // If the groupid doesn't exist, just skip the initialization and return false
            auto groupTypeConf = parent_component_->actor()->GetGroupType(group_id_.group_type_id);
            if (groupTypeConf == nullptr)
                return false;

            group_type_conf_ = *groupTypeConf;


            // Default port for the group. Reserved for RIAPS internal communication protocols
            GroupPortPub internalPubConfig;
            internalPubConfig.message_type = INTERNAL_MESSAGETYPE;
            internalPubConfig.is_local     = false;
            internalPubConfig.port_name    = INTERNAL_PUB_NAME;

            vector<GroupService> initializedServices;

            group_pubport_ = std::shared_ptr<ports::GroupPublisherPort>(new ports::GroupPublisherPort(internalPubConfig, parent_component_));
            initializedServices.push_back(group_pubport_->GetGroupService());
            group_ports_[group_pubport_->port_socket()] = group_pubport_;


            GroupPortSub internalSubConfig;
            internalSubConfig.message_type = INTERNAL_MESSAGETYPE;
            internalSubConfig.is_local     = false;
            internalSubConfig.port_name    = INTERNAL_SUB_NAME;

            group_subport_ = shared_ptr<ports::GroupSubscriberPort>(new ports::GroupSubscriberPort(internalSubConfig, parent_component_));
            group_ports_[group_subport_->port_socket()] = group_subport_;

            // Initialize the zpoller and add the group sub port
            group_poller_ = zpoller_new(const_cast<zsock_t*>(group_subport_->port_socket()), nullptr);

            // Initialize user defined publishers
            for(auto& portDeclaration : group_type_conf_.group_type_ports.pubs){
                auto newPubPort = std::shared_ptr<ports::GroupPublisherPort>(new ports::GroupPublisherPort(portDeclaration, parent_component_));
                initializedServices.push_back(newPubPort->GetGroupService());
                group_ports_[newPubPort->port_socket()]=std::move(newPubPort);

            }

            // Initialize user defined subscribers
            for(auto& portDeclaration : group_type_conf_.group_type_ports.subs){
                auto newSubPort = shared_ptr<ports::GroupSubscriberPort>(new ports::GroupSubscriberPort(portDeclaration, parent_component_));
                zpoller_add(group_poller_, const_cast<zsock_t*>(newSubPort->port_socket()));
                group_ports_[newSubPort->port_socket()] = std::move(newSubPort);

            }

            bool hasJoined = Disco::JoinGroup(
                    parent_component_->actor()->application_name(),
                    parent_component_->ComponentUuid(),
                    group_id_,
                    initializedServices);

            // Setup leader election
            if (hasJoined && group_type_conf_.has_leader) {
                group_leader_ = std::unique_ptr<riaps::groups::GroupLead>(
                        new GroupLead(this, &known_nodes_)
                );
                group_leader_->SetOnLeaderChanged([this](const std::string& newLeader){
                    logger_->debug("Leader changed: {}", newLeader);
                });
            }

            // Register all of the publishers
            return hasJoined;
        }




        bool Group::SendInternalMessage(capnp::MallocMessageBuilder &message) {
            return SendMessage(message, INTERNAL_PUB_NAME);
        }

        bool Group::SendMessageToLeader(capnp::MallocMessageBuilder &message) {
            if (leader_id() == "") return false;

            zframe_t* frame;
            frame << message;

            capnp::MallocMessageBuilder builder;
            auto msgGroupInternals = builder.initRoot<riaps::distrcoord::GroupInternals>();
            auto msgHeader = msgGroupInternals.initMessageToLeader();
            msgHeader.setSourceComponentId(parent_component()->ComponentUuid());

            zframe_t* header;
            header << builder;

            zmsg_t* zmsg = zmsg_new();
            zmsg_add(zmsg, header);
            zmsg_add(zmsg, frame);

            return SendMessage(&zmsg, INTERNAL_PUB_NAME);
        }

        bool Group::ProposeValueToLeader(capnp::MallocMessageBuilder &message, const std::string &proposeId) {
            bool hasActiveLeader = leader_id() != "";
            if (!hasActiveLeader){
                logger_->error("ProposeValueToLeader(), no active leader, send failed");
                return false;
            }

            zframe_t* frame;
            frame << message;

            capnp::MallocMessageBuilder builder;
            auto msgGroupInternals = builder.initRoot<riaps::distrcoord::GroupInternals>();
            auto msgCons = msgGroupInternals.initConsensus();
            msgCons.setVoteType(riaps::distrcoord::Consensus::VoteType::VALUE);

            auto msgPropLeader = msgCons.initProposeToLeader();
            msgPropLeader.setProposeId(proposeId);
            msgCons.setSourceComponentId(parent_component_id());

            zframe_t* header;
            header << builder;

            zmsg_t* zmsg = zmsg_new();
            zmsg_add(zmsg, header);
            zmsg_add(zmsg, frame);

            bool rc = SendMessage(&zmsg, INTERNAL_PUB_NAME);
            if (!rc)
                logger_->error("ProposeValueToLeader() failed");
            else
                logger_->debug("ProposeValueToLeader() proposeId: {}, leader_id: {}, srcComp: {}", proposeId,
                               leader_id(),
                               parent_component_id());
            return rc;
        }

        bool Group::ProposeActionToLeader(const std::string& proposeId,
                                          const std::string &actionId,
                                          const timespec &absTime) {
            bool hasActiveLeader = leader_id() != "";
            if (!hasActiveLeader){
                logger_->error("ProposeActionToLeader(), no active leader, send failed");
                return false;
            }


            capnp::MallocMessageBuilder builder;
            auto msgGroupInternals = builder.initRoot<riaps::distrcoord::GroupInternals>();
            auto msgCons = msgGroupInternals.initConsensus();
            auto msgPropLeader = msgCons.initProposeToLeader();

            msgCons.setVoteType(riaps::distrcoord::Consensus::VoteType::ACTION);

            msgPropLeader.setProposeId(proposeId);
            msgCons.setSourceComponentId(parent_component_id());

            auto msgTsyncA = msgCons.initTsyncCoordA();
            msgTsyncA.setActionId(actionId);

            auto msgTime = msgTsyncA.initTime();
            msgTime.setTvSec(absTime.tv_sec);
            msgTime.setTvNsec(absTime.tv_nsec);

            zframe_t* header;
            header << builder;

            zmsg_t* zmsg = zmsg_new();
            zmsg_add(zmsg, header);

            bool rc = SendMessage(&zmsg, INTERNAL_PUB_NAME);

            if (!rc)
                logger_->error("ProposeActionToLeader() failed");
            else
                logger_->debug("ProposeActionToLeader() proposeId: {}, leader_id: {}, srcComp: {}", proposeId,
                              leader_id(),
                              parent_component()->ComponentUuid());
            return rc;
        }

        bool Group::SendLeaderMessage(capnp::MallocMessageBuilder &message) {
            if (leader_id()!= parent_component_id()) return false;
            capnp::MallocMessageBuilder builder;
            auto intMessage = builder.initRoot<riaps::distrcoord::GroupInternals>();
            auto grpMessage = intMessage.initGroupMessage();
            grpMessage.setSourceComponentId(parent_component_id());

            zframe_t* header;
            header << builder;

            zmsg_t* zmsg = zmsg_new();
            zmsg_add(zmsg, header);

            zframe_t* body;
            body << message;
            zmsg_add(zmsg, body);
            return SendMessage(&zmsg, INTERNAL_PUB_NAME);
        }



        bool Group::SendMessage(capnp::MallocMessageBuilder& message, const std::string& portName){

            // If the port is not specified, send on the internal port
            if (portName != "") {
                for (auto it = group_ports_.begin(); it!=group_ports_.end(); it++){
                    auto currentPort = it->second->AsGroupPublishPort();
                    if (currentPort == nullptr) continue;
                    if (currentPort->GetConfig()->port_name != portName) continue;

                    return currentPort->Send(message);

                }
            } else {
                capnp::MallocMessageBuilder builder;
                auto intMessage = builder.initRoot<riaps::distrcoord::GroupInternals>();
                auto grpMessage = intMessage.initGroupMessage();
                grpMessage.setSourceComponentId(parent_component_id());

                zframe_t* header;
                zframe_t* content;

                header << builder;
                content << message;

                // append() takes the ownership, zframe_t*, zmsg_t* will be destroyed after send();
                zmsg_t* zmsg = zmsg_new();
                zmsg_append(zmsg, &header);
                zmsg_append(zmsg, &content);
                return SendMessage(&zmsg, INTERNAL_PUB_NAME);
            }

            return false;
        }

        bool Group::SendMessage(zmsg_t** message, const std::string& portName){
            for (auto it = group_ports_.begin(); it!=group_ports_.end(); it++){
                auto currentPort = it->second->AsGroupPublishPort();
                if (currentPort == nullptr) continue;
                if (currentPort->GetConfig()->port_name != portName) continue;

                return currentPort->Send(message);

            }

            return false;
        }

        std::shared_ptr<std::set<std::string>> Group::GetKnownComponents() {
            std::shared_ptr<std::set<std::string>> result(new std::set<std::string>());

            for (auto& n : known_nodes_){
                if (n.second.IsTimeout()) continue;
                result->emplace(n.first);
            }
//            std::transform(known_nodes_.begin(),
//                           known_nodes_.end(),
//                           std::back_inserter(*result),
//                           [](const std::pair<std::string, Timeout >& p) -> std::string {
//                               return p.first;
//                           });

            return result;
        }

        void Group::ConnectToNewServices(riaps::discovery::GroupUpdate::Reader &msgGroupUpdate) {
            for (int i =0; i<msgGroupUpdate.getServices().size(); i++){
                string message_type = msgGroupUpdate.getServices()[i].getMessageType().cStr();

//                // RIAPS port
//                if (message_type == INTERNAL_MESSAGETYPE){
//                    std::string address = msgGroupUpdate.getServices()[i].getAddress().cStr();
//                    address = "tcp://" + address;
//                    _groupSubPort->ConnectToPublihser(address);
//                    continue;
//                }

                for (auto& groupPort : group_ports_){
                    auto subscriberPort = groupPort.second->AsGroupSubscriberPort();
                    if (subscriberPort == nullptr) continue;
                    if (subscriberPort->GetConfig()->message_type == message_type){
                        std::string address = msgGroupUpdate.getServices()[i].getAddress().cStr();
                        address = "tcp://" + address;
                        subscriberPort->ConnectToPublihser(address);
                    }
                }
            }
        }

        bool Group::SendPing() {
            //m_logger->debug(">>PING>>");
            ping_timeout_.Reset();
            return SendHeartBeat(dc::HeartBeatType::PING);
        }

        bool Group::SendPong() {
            //m_logger->debug(">>PONG>>");
            return SendHeartBeat(dc::HeartBeatType::PONG);
        }

        bool Group::SendPingWithPeriod() {

            /**
             * If no known node, sending periodically.
             * Else Send ping ONLY if there is a suspicoius component (timeout exceeded)
             */
            if (known_nodes_.size() == 0 && ping_timeout_.IsTimeout()){
                ping_timeout_.Reset();
                return SendPing();
            }
            else {
                for (auto it = known_nodes_.begin(); it != known_nodes_.end(); it++) {
                    if (it->second.IsTimeout()&& ping_timeout_.IsTimeout()) {
                        ping_timeout_.Reset();
                        return SendPing();
                    }
                }
            }

            //if (ping_timeout_.IsTimeout()){
            //    return SendPing();
            //}
//            int64_t currentTime = zclock_mono();
//
//            if (_lastPingSent == 0)
//                _lastPingSent = currentTime;
//
//            // If the ping period has been reached
//            if ((currentTime - _lastPingSent) > _pingPeriod){
//                return SendPing();
//            }
            return false;
        }


        bool Group::SendHeartBeat(riaps::distrcoord::HeartBeatType type) {
            capnp::MallocMessageBuilder builder;
            auto internal = builder.initRoot<riaps::distrcoord::GroupInternals>();
            auto heartbeat = internal.initGroupHeartBeat();

            heartbeat.setHeartBeatType(type);
            heartbeat.setSourceComponentId(this->parent_component_->ComponentUuid());

            return group_pubport_->Send(builder);
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

        void Group::FetchNextMessage() {

            void* which = zpoller_wait(group_poller_, 1);
            if (which == nullptr){
                // No incoming message, update the leader
                if (group_type_conf_.has_leader) {
                    group_leader_->Update();
                }
                return;
            }

            // Look for the port
            ports::PortBase* currentPort = nullptr;
            try{
                currentPort = group_ports_.at(static_cast<zsock_t*>(which)).get();
            } catch (std::out_of_range& e){
                return;
            }

            riaps::ports::GroupSubscriberPort* subscriberPort = currentPort->AsGroupSubscriberPort();

            // If the port is not a subscriber port (in theory this is impossible)
            if (subscriberPort == nullptr)  return;

            // Delete the previous message if there is any
//            if (_lastFrame != nullptr) {
//                zframe_destroy(&_lastFrame);
//                _lastFrame = nullptr;
//            };

            zmsg_t* msg = zmsg_recv(const_cast<zsock_t*>(subscriberPort->port_socket()));
            zframe_t* firstFrame;
            capnp::FlatArrayMessageReader frmReader(nullptr);

            if (msg) {

                auto msgPtr = std::shared_ptr<zmsg_t>(msg, [](zmsg_t* z){zmsg_destroy(&z);});

                // _lstFrame owns the data. Must be preserved until the next message
                firstFrame  = zmsg_pop(msg);
                auto framePtr = std::shared_ptr<zframe_t>(firstFrame, [](zframe_t* z){zframe_destroy(&z);});
                (*firstFrame) >> frmReader;
                //size_t size = zframe_size(_lastFrame);
                //byte *data  = zframe_data(_lastFrame);

                //kj::ArrayPtr<const capnp::word> capnp_data(reinterpret_cast<const capnp::word *>(data), size / sizeof(capnp::word));


                //messageReader.reset(new capnp::FlatArrayMessageReader(capnp_data));

                // Internal port, handle it here and don't send any notifications to the caller
                if (subscriberPort == group_subport_.get()){
                    //capnp::FlatArrayMessageReader reader(capnp_data);

                    auto internal = frmReader.getRoot<riaps::distrcoord::GroupInternals>();
                    if (internal.hasGroupMessage()) {
                        // Read the content
                        zframe_t* messageFrame;
                        messageFrame = zmsg_pop(msg);
                        if (messageFrame) {
                            auto msgFrmPtr = std::shared_ptr<zframe_t>(messageFrame, [](zframe_t* z){zframe_destroy(&z);});
                            (*messageFrame) >> frmReader;
                            //frmReader = capnpReader;
                            parent_component_->OnGroupMessage(group_id_, frmReader, nullptr);
                            return;
                        }
                    }
                    else if (internal.hasGroupHeartBeat()) {
                        auto groupHeartBeat = internal.getGroupHeartBeat();
                        auto it = known_nodes_.find(groupHeartBeat.getSourceComponentId());

                        // New node, set the timeout
                        if (it == known_nodes_.end()) {
                            known_nodes_[groupHeartBeat.getSourceComponentId().cStr()] =
                                    Timeout<std::chrono::milliseconds>(timeout_distribution_(random_generator_));
                        } else
                            it->second.Reset(timeout_distribution_(random_generator_));

                        if (groupHeartBeat.getHeartBeatType() == riaps::distrcoord::HeartBeatType::PING) {
                            //m_logger->debug("<<PING<<");
                            SendPong();
                            return;
                        } else if (groupHeartBeat.getHeartBeatType() == riaps::distrcoord::HeartBeatType::PONG) {
                            //logger_->debug("<<PONG<<");
                            return;
                        }
                    } else if (internal.hasLeaderElection()){
                        auto msgLeader = internal.getLeaderElection();
                        group_leader_->Update(msgLeader);
                        return;
                    } else if (internal.hasMessageToLeader()){
                        auto msgLeader = internal.getMessageToLeader();
                        if (leader_id() != parent_component()->ComponentUuid()) return;

                        zframe_t* leaderMsgFrame = zmsg_pop(msg);
                        if (!leaderMsgFrame) return;
                        auto leaderMsgPtr = std::shared_ptr<zframe_t>(leaderMsgFrame, [](zframe_t* z){zframe_destroy(&z);});
                        capnp::FlatArrayMessageReader capnpLeaderMsg(nullptr);
                        (*leaderMsgFrame) >> capnpLeaderMsg;

                        parent_component_->OnMessageToLeader(group_id_, capnpLeaderMsg);

                        return;
                    } else if (internal.hasConsensus()) {
                        auto msgCons = internal.getConsensus();
                       // m_logger->debug("DC message arrived from {}", msgDistCoord.getSourceComponentId().cStr());

//                        // The current component is the leader
                        if (leader_id() == parent_component_id()) {
                            //m_logger->debug("DC message arrived and this component is the leader");

//                            // Propose arrived to the leader. Leader forwards it to every groupmember.
                            // TODO: We may not need the forwarding step, since everybody got the message
                            if (msgCons.hasProposeToLeader()){
                                auto msgPropose = msgCons.getProposeToLeader();

                                // Value is proposed
                                if (msgCons.getVoteType() == riaps::distrcoord::Consensus::VoteType::VALUE){
                                    zframe_t* proposeFrame;
                                    proposeFrame = zmsg_pop(msg);
                                    logger_->info("Message proposed to the leader, proposeId: {}", msgPropose.getProposeId().cStr());
                                    group_leader_->OnProposeFromClient(msgPropose, &proposeFrame);
                                    if (proposeFrame!= nullptr)
                                        zframe_destroy(&proposeFrame);
                                }

                                    // Action is proposed to the leader
                                else if (msgCons.getVoteType() == riaps::distrcoord::Consensus::VoteType::ACTION) {
                                    auto msgTsca = msgCons.getTsyncCoordA();
                                    logger_->info("Action proposed to the leader, proposeId: {}, actionId: {}",
                                                  msgPropose.getProposeId().cStr(),
                                                  msgTsca.getActionId().cStr());
                                    group_leader_->OnActionProposeFromClient(msgPropose,msgTsca);
                                }

//                            // Vote arrived, count the votes and announce the results (if any)
                            } else if (msgCons.hasVote()){
                                //m_logger->info("Vote arrived to the leader");
                                auto msgVote = msgCons.getVote();
                                group_leader_->OnVote(msgVote, msgCons.getSourceComponentId());
                            }
                        }
//                        // The current component is not a leader
                        else {
                            //m_logger->debug("DC message arrived and this component is not the leader");
//                            // propose by the leader, must vote on something
                            if (msgCons.hasProposeToClients()) {
                                auto msgPropose = msgCons.getProposeToClients();
                                //m_logger->debug("Message proposed to the client, proposeId: {}", msgPropose.getProposeId().cStr());

                                if (msgCons.getVoteType() == riaps::distrcoord::Consensus::VoteType::VALUE) {
                                    zframe_t *proposeFrame;
                                    proposeFrame = zmsg_pop(msg);
                                    capnp::FlatArrayMessageReader *reader;
                                    (*proposeFrame) >> reader;
                                    parent_component_->OnPropose(group_id_, msgPropose.getProposeId(), *reader);
                                } else if (msgCons.getVoteType() == riaps::distrcoord::Consensus::VoteType::ACTION) {
                                    timespec t{
                                            msgCons.getTsyncCoordA().getTime().getTvSec(),
                                            msgCons.getTsyncCoordA().getTime().getTvNsec()
                                    };

                                    if (t.tv_sec!=msgCons.getTsyncCoordA().getTime().getTvSec())
                                        logger_->error("tv_sec!=TvSec");
                                    if (t.tv_nsec!=msgCons.getTsyncCoordA().getTime().getTvNsec())
                                        logger_->error("tv_nsec!=TvNsec");

                                    parent_component_->OnActionPropose(group_id_,
                                                                      msgPropose.getProposeId(),
                                                                      msgCons.getTsyncCoordA().getActionId(),
                                                                      t);
                                }
                            } else if (msgCons.hasAnnounce()) {
                                auto msgAnnounce = msgCons.getAnnounce();
                                parent_component_->OnAnnounce(group_id_,
                                                             msgAnnounce.getProposeId(),
                                                             msgAnnounce.getVoteResult() == riaps::distrcoord::Consensus::VoteResults::ACCEPTED);
                            }
                        }

                        return;
                    }
                }


            }

            //zmsg_destroy(&msg);
            return;
        }

        bool Group::SendVote(const std::string &propose_id, bool accept) {
            capnp::MallocMessageBuilder builder;
            auto msgInt = builder.initRoot<riaps::distrcoord::GroupInternals>();
            auto msgCons = msgInt.initConsensus();
            msgCons.setSourceComponentId(parent_component()->ComponentUuid());
            auto msgVote = msgCons.initVote();
            msgVote.setProposeId(propose_id);
            if (accept)
                msgVote.setVoteResult(riaps::distrcoord::Consensus::VoteResults::ACCEPTED);
            else
                msgVote.setVoteResult(riaps::distrcoord::Consensus::VoteResults::REJECTED);

            return SendInternalMessage(builder);
        }

        Group::~Group() {
//            if (_lastFrame!= nullptr){
//                zframe_destroy(&_lastFrame);
//                _lastFrame=nullptr;
//            }
            if (group_poller_ != nullptr)
                zpoller_destroy(&group_poller_);
        }
    }
}
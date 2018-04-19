//
// Created by istvan on 10/3/17.
//

#include <groups/r_group.h>
#include <messaging/distcoord.capnp.h>
#include <capnp/common.h>

/*10s in msec*/
#define PING_BASE_PERIOD 10*1000

namespace dc = riaps::distrcoord;

namespace riaps{

    class ComponentBase;

    namespace groups{

        using namespace riaps;

        bool GroupId::operator<(const GroupId &other) const {
            if (groupTypeId == other.groupTypeId){
                return groupName < other.groupName;
            }
            return groupTypeId<other.groupTypeId;
        }

        bool GroupId::operator==(const GroupId &other) const {
            return groupTypeId == other.groupTypeId && groupName == other.groupName;
        }

        std::string Group::GetLeaderId() const {
            if (m_groupTypeConf.hasLeader && _groupLeader != nullptr){
                return _groupLeader->GetLeaderId();
            }
            return "";
        }

        Group::Group(const GroupId &groupId, ComponentBase* parentComponent) :
                m_groupId(groupId),
                _parentComponent(parentComponent),
                m_groupPubPort(nullptr),
                m_groupSubPort(nullptr),
                //_lastFrame(nullptr),
                _groupLeader(nullptr),
                _groupPoller(nullptr) {
            _pingCounter = 0;
            _logger = spd::get(parentComponent->GetConfig().component_name);
            _pingTimeout = Timeout<std::milli>(duration<int,std::milli>(PING_BASE_PERIOD));

            _generator = std::mt19937(_rd());
            _distrNodeTimeout = std::uniform_int_distribution<int>(PING_BASE_PERIOD*1.1, PING_BASE_PERIOD*2);
        }

        bool Group::InitGroup() {

            // If the groupid doesn't exist, just skip the initialization and return false
            auto groupTypeConf = ::riaps::Actor::GetRunningActor()->GetGroupType(m_groupId.groupTypeId);
            if (groupTypeConf == nullptr)
                return false;

            m_groupTypeConf = *groupTypeConf;


            // Default port for the group. Reserved for RIAPS internal communication protocols
            group_port_pub internalPubConfig;
            internalPubConfig.messageType = INTERNAL_MESSAGETYPE;
            internalPubConfig.isLocal     = false;
            internalPubConfig.portName    = INTERNAL_PUB_NAME;

            std::vector<GroupService> initializedServices;

            m_groupPubPort = std::shared_ptr<ports::GroupPublisherPort>(new ports::GroupPublisherPort(internalPubConfig, _parentComponent));
            initializedServices.push_back(m_groupPubPort->GetGroupService());
            _groupPorts[m_groupPubPort->GetSocket()] = m_groupPubPort;


            group_port_sub internalSubConfig;
            internalSubConfig.messageType = INTERNAL_MESSAGETYPE;
            internalSubConfig.isLocal     = false;
            internalSubConfig.portName    = INTERNAL_SUB_NAME;

            m_groupSubPort = std::shared_ptr<ports::GroupSubscriberPort>(new ports::GroupSubscriberPort(internalSubConfig, _parentComponent));
            _groupPorts[m_groupSubPort->GetSocket()] = m_groupSubPort;



            // Initialize the zpoller and add the group sub port
            _groupPoller = zpoller_new(const_cast<zsock_t*>(m_groupSubPort->GetSocket()), nullptr);

            // Initialize user defined publishers
            for(auto& portDeclaration : m_groupTypeConf.groupTypePorts.pubs){
                auto newPubPort = std::shared_ptr<ports::GroupPublisherPort>(new ports::GroupPublisherPort(portDeclaration, _parentComponent));
                initializedServices.push_back(newPubPort->GetGroupService());
                _groupPorts[newPubPort->GetSocket()]=std::move(newPubPort);

            }

            // Initialize user defined subscribers
            for(auto& portDeclaration : m_groupTypeConf.groupTypePorts.subs){
                auto newSubPort = std::shared_ptr<ports::GroupSubscriberPort>(new ports::GroupSubscriberPort(portDeclaration, _parentComponent));
                zpoller_add(_groupPoller, const_cast<zsock_t*>(newSubPort->GetSocket()));
                _groupPorts[newSubPort->GetSocket()] = std::move(newSubPort);

            }

            bool hasJoined = joinGroup(riaps::Actor::GetRunningActor()->getApplicationName(),
                                       _parentComponent->GetCompUuid(),
                                       m_groupId,
                                       initializedServices);

            // Setup leader election
            if (hasJoined && m_groupTypeConf.hasLeader) {
                _groupLeader = std::unique_ptr<riaps::groups::GroupLead>(
                        new GroupLead(this, &_knownNodes)
                );
                _groupLeader->SetOnLeaderChanged([this](const std::string& newLeader){
                    _logger->debug("Leader changed: {}", newLeader);
                });
            }

            // Register all of the publishers
            return hasJoined;
        }




        bool Group::SendInternalMessage(capnp::MallocMessageBuilder &message) {
            return SendMessage(message, INTERNAL_PUB_NAME);
        }

        bool Group::SendMessageToLeader(capnp::MallocMessageBuilder &message) {
            if (GetLeaderId() == "") return false;

            zframe_t* frame;
            frame << message;

            capnp::MallocMessageBuilder builder;
            auto msgGroupInternals = builder.initRoot<riaps::distrcoord::GroupInternals>();
            auto msgHeader = msgGroupInternals.initMessageToLeader();
            msgHeader.setSourceComponentId(GetParentComponent()->GetCompUuid());

            zframe_t* header;
            header << builder;

            zmsg_t* zmsg = zmsg_new();
            zmsg_add(zmsg, header);
            zmsg_add(zmsg, frame);

            return SendMessage(&zmsg, INTERNAL_PUB_NAME);
        }

        bool Group::ProposeValueToLeader(capnp::MallocMessageBuilder &message, const std::string &proposeId) {
            bool hasActiveLeader = GetLeaderId() != "";
            if (!hasActiveLeader){
                _logger->error("ProposeValueToLeader(), no active leader, send failed");
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
            msgCons.setSourceComponentId(GetParentComponentId());

            zframe_t* header;
            header << builder;

            zmsg_t* zmsg = zmsg_new();
            zmsg_add(zmsg, header);
            zmsg_add(zmsg, frame);

            bool rc = SendMessage(&zmsg, INTERNAL_PUB_NAME);
            _logger->error_if(!rc, "ProposeValueToLeader() failed");
            _logger->debug_if(rc, "ProposeValueToLeader() proposeId: {}, leaderId: {}, srcComp: {}", proposeId, GetLeaderId(), GetParentComponentId());
            return rc;
        }

        bool Group::ProposeActionToLeader(const std::string& proposeId,
                                          const std::string &actionId,
                                          const timespec &absTime) {
            bool hasActiveLeader = GetLeaderId() != "";
            if (!hasActiveLeader){
                _logger->error("ProposeActionToLeader(), no active leader, send failed");
                return false;
            }


            capnp::MallocMessageBuilder builder;
            auto msgGroupInternals = builder.initRoot<riaps::distrcoord::GroupInternals>();
            auto msgCons = msgGroupInternals.initConsensus();
            auto msgPropLeader = msgCons.initProposeToLeader();

            msgCons.setVoteType(riaps::distrcoord::Consensus::VoteType::ACTION);

            msgPropLeader.setProposeId(proposeId);
            msgCons.setSourceComponentId(GetParentComponentId());

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
            _logger->error_if(!rc, "ProposeActionToLeader() failed");
            _logger->debug_if(rc, "ProposeActionToLeader() proposeId: {}, leaderId: {}, srcComp: {}", proposeId, GetLeaderId(), GetParentComponent()->GetCompUuid());
            return rc;
        }

        bool Group::SendLeaderMessage(capnp::MallocMessageBuilder &message) {
            if (GetLeaderId()!=GetParentComponentId()) return false;
            capnp::MallocMessageBuilder builder;
            auto intMessage = builder.initRoot<riaps::distrcoord::GroupInternals>();
            auto grpMessage = intMessage.initGroupMessage();
            grpMessage.setSourceComponentId(GetParentComponentId());

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
                for (auto it = _groupPorts.begin(); it!=_groupPorts.end(); it++){
                    auto currentPort = it->second->AsGroupPublishPort();
                    if (currentPort == nullptr) continue;
                    if (currentPort->GetConfig()->portName != portName) continue;

                    return currentPort->Send(message);

                }
            } else {
                capnp::MallocMessageBuilder builder;
                auto intMessage = builder.initRoot<riaps::distrcoord::GroupInternals>();
                auto grpMessage = intMessage.initGroupMessage();
                grpMessage.setSourceComponentId(GetParentComponentId());

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
            for (auto it = _groupPorts.begin(); it!=_groupPorts.end(); it++){
                auto currentPort = it->second->AsGroupPublishPort();
                if (currentPort == nullptr) continue;
                if (currentPort->GetConfig()->portName != portName) continue;

                return currentPort->Send(message);

            }

            return false;
        }

        std::shared_ptr<std::set<std::string>> Group::GetKnownComponents() {
            std::shared_ptr<std::set<std::string>> result(new std::set<std::string>());

            for (auto& n : _knownNodes){
                if (n.second.IsTimeout()) continue;
                result->emplace(n.first);
            }
//            std::transform(_knownNodes.begin(),
//                           _knownNodes.end(),
//                           std::back_inserter(*result),
//                           [](const std::pair<std::string, Timeout >& p) -> std::string {
//                               return p.first;
//                           });

            return result;
        }

        void Group::ConnectToNewServices(riaps::discovery::GroupUpdate::Reader &msgGroupUpdate) {
            for (int i =0; i<msgGroupUpdate.getServices().size(); i++){
                std::string messageType = msgGroupUpdate.getServices()[i].getMessageType().cStr();

//                // RIAPS port
//                if (messageType == INTERNAL_MESSAGETYPE){
//                    std::string address = msgGroupUpdate.getServices()[i].getAddress().cStr();
//                    address = "tcp://" + address;
//                    _groupSubPort->ConnectToPublihser(address);
//                    continue;
//                }

                for (auto& groupPort : _groupPorts){
                    auto subscriberPort = groupPort.second->AsGroupSubscriberPort();
                    if (subscriberPort == nullptr) continue;
                    if (subscriberPort->GetConfig()->messageType == messageType){
                        std::string address = msgGroupUpdate.getServices()[i].getAddress().cStr();
                        address = "tcp://" + address;
                        subscriberPort->ConnectToPublihser(address);
                    }
                }
            }
        }

        bool Group::SendPing() {
            //m_logger->debug(">>PING>>");
            _pingTimeout.Reset();
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
            if (_knownNodes.size() == 0 && _pingTimeout.IsTimeout()){
                _pingTimeout.Reset();
                return SendPing();
            }
            else {
                for (auto it = _knownNodes.begin(); it != _knownNodes.end(); it++) {
                    if (it->second.IsTimeout()&& _pingTimeout.IsTimeout()) {
                        _pingTimeout.Reset();
                        return SendPing();
                    }
                }
            }

            //if (_pingTimeout.IsTimeout()){
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
            heartbeat.setSourceComponentId(this->_parentComponent->GetCompUuid());

            return m_groupPubPort->Send(builder);
        }

        const ComponentBase* Group::GetParentComponent() const {
            return _parentComponent;
        }

        const std::string Group::GetParentComponentId() const {
            return GetParentComponent()->GetCompUuid();
        }

        uint16_t Group::GetMemberCount() {
            DeleteTimeoutNodes();
            return _knownNodes.size();
        }

        uint32_t Group::DeleteTimeoutNodes() {
            uint32_t deleted=0;
            for(auto it = std::begin(_knownNodes); it != std::end(_knownNodes);)
            {
                if ((*it).second.IsTimeout())
                {
                    it = _knownNodes.erase(it);
                    deleted++;
                }
                else
                    ++it;
            }
            return deleted;
        }

//        ports::GroupSubscriberPort* Group::FetchNextMessage(std::shared_ptr<capnp::FlatArrayMessageReader>& messageReader) {
//
//            void* which = zpoller_wait(_groupPoller, 1);
//            if (which == nullptr){
//                // No incoming message, update the leader
//                if (_groupTypeConf.hasLeader) {
//                    _groupLeader->Update();
//                }
//                return nullptr;
//            }
//
//            // Look for the port
//            ports::PortBase* currentPort = nullptr;
//            try{
//                currentPort = _groupPorts.at(static_cast<zsock_t*>(which)).get();
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
//                            //_parentComponent->OnGroupMessage(_groupId, *capnpReader.get(), nullptr);
//                        }
//                    }
//                    else if (internal.hasGroupHeartBeat()) {
//                        auto groupHeartBeat = internal.getGroupHeartBeat();
//                        auto it = _knownNodes.find(groupHeartBeat.getSourceComponentId());
//
//                        // New node, set the timeout
//                        if (it == _knownNodes.end()) {
//                            _knownNodes[groupHeartBeat.getSourceComponentId().cStr()] =
//                                    Timeout<std::milli>(duration<int, std::milli>(_distrNodeTimeout(_generator)));
//                        } else
//                            it->second.Reset(duration<int, std::milli>(_distrNodeTimeout(_generator)));
//
//                        if (groupHeartBeat.getHeartBeatType() == riaps::distrcoord::HeartBeatType::PING) {
//                            //_logger->debug("<<PING<<");
//                            SendPong();
//                            return nullptr;
//                        } else if (groupHeartBeat.getHeartBeatType() == riaps::distrcoord::HeartBeatType::PONG) {
//                            //_logger->debug("<<PONG<<");
//                            return nullptr;
//                        }
//                    } else if (internal.hasLeaderElection()){
//                        auto msgLeader = internal.getLeaderElection();
//                        _groupLeader->Update(msgLeader);
//                        return nullptr;
//                    } else if (internal.hasMessageToLeader()){
//                        auto msgLeader = internal.getMessageToLeader();
//                        if (GetLeaderId() != GetParentComponent()->GetCompUuid()) return nullptr;
//                        _logger->debug("Message to the leader arrived!");
//
//                        return nullptr;
//                    } else if (internal.hasConsensus()) {
//                        auto msgCons = internal.getConsensus();
//                       // _logger->debug("DC message arrived from {}", msgDistCoord.getSourceComponentId().cStr());
//
////                        // The current component is the leader
//                        if (GetLeaderId() == GetParentComponentId()) {
//                            //_logger->debug("DC message arrived and this component is the leader");
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
//                                    _logger->info("Message proposed to the leader, proposeId: {}", msgPropose.getProposeId().cStr());
//                                    _groupLeader->OnProposeFromClient(msgPropose, &proposeFrame);
//                                    if (proposeFrame!= nullptr)
//                                        zframe_destroy(&proposeFrame);
//                                }
//
//                                // Action is proposed to the leader
//                                else if (msgCons.getVoteType() == riaps::distrcoord::Consensus::VoteType::ACTION) {
//                                    auto msgTsca = msgCons.getTsyncCoordA();
//                                    _logger->info("Action proposed to the leader, proposeId: {}, actionId: {}",
//                                                  msgPropose.getProposeId().cStr(),
//                                                  msgTsca.getActionId().cStr());
//                                    _groupLeader->OnActionProposeFromClient(msgPropose,msgTsca);
//                                }
//
////                            // Vote arrived, count the votes and announce the results (if any)
//                            } else if (msgCons.hasVote()){
//                                //_logger->info("Vote arrived to the leader");
//                                auto msgVote = msgCons.getVote();
//                                _groupLeader->OnVote(msgVote, msgCons.getSourceComponentId());
//                            }
//                        }
////                        // The current component is not a leader
//                        else {
//                            //_logger->debug("DC message arrived and this component is not the leader");
////                            // propose by the leader, must vote on something
//                            if (msgCons.hasProposeToClients()) {
//                                auto msgPropose = msgCons.getProposeToClients();
//                                //_logger->debug("Message proposed to the client, proposeId: {}", msgPropose.getProposeId().cStr());
//
//                                if (msgCons.getVoteType() == riaps::distrcoord::Consensus::VoteType::VALUE) {
//                                    zframe_t *proposeFrame;
//                                    proposeFrame = zmsg_pop(msg);
//                                    capnp::FlatArrayMessageReader *reader;
//                                    (*proposeFrame) >> reader;
//                                    _parentComponent->OnPropose(_groupId, msgPropose.getProposeId(), *reader);
//                                } else if (msgCons.getVoteType() == riaps::distrcoord::Consensus::VoteType::ACTION) {
//                                    timespec t{
//                                            msgCons.getTsyncCoordA().getTime().getTvSec(),
//                                            msgCons.getTsyncCoordA().getTime().getTvNsec()
//                                    };
//
//                                    _logger->error_if(t.tv_sec!=msgCons.getTsyncCoordA().getTime().getTvSec(),"tv_sec!=TvSec");
//                                    _logger->error_if(t.tv_nsec!=msgCons.getTsyncCoordA().getTime().getTvNsec(),"tv_nsec!=TvNsec");
//
//                                    _parentComponent->OnActionPropose(_groupId,
//                                                                      msgPropose.getProposeId(),
//                                                                      msgCons.getTsyncCoordA().getActionId(),
//                                                                      t);
//                                }
//                            } else if (msgCons.hasAnnounce()) {
//                                auto msgAnnounce = msgCons.getAnnounce();
//                                _parentComponent->OnAnnounce(_groupId,
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

            void* which = zpoller_wait(_groupPoller, 1);
            if (which == nullptr){
                // No incoming message, update the leader
                if (m_groupTypeConf.hasLeader) {
                    _groupLeader->Update();
                }
                return;
            }

            // Look for the port
            ports::PortBase* currentPort = nullptr;
            try{
                currentPort = _groupPorts.at(static_cast<zsock_t*>(which)).get();
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

            zmsg_t* msg = zmsg_recv(const_cast<zsock_t*>(subscriberPort->GetSocket()));
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
                if (subscriberPort == m_groupSubPort.get()){
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
                            _parentComponent->OnGroupMessage(m_groupId, frmReader, nullptr);
                            return;
                        }
                    }
                    else if (internal.hasGroupHeartBeat()) {
                        auto groupHeartBeat = internal.getGroupHeartBeat();
                        auto it = _knownNodes.find(groupHeartBeat.getSourceComponentId());

                        // New node, set the timeout
                        if (it == _knownNodes.end()) {
                            _knownNodes[groupHeartBeat.getSourceComponentId().cStr()] =
                                    Timeout<std::milli>(duration<int, std::milli>(_distrNodeTimeout(_generator)));
                        } else
                            it->second.Reset(duration<int, std::milli>(_distrNodeTimeout(_generator)));

                        if (groupHeartBeat.getHeartBeatType() == riaps::distrcoord::HeartBeatType::PING) {
                            //m_logger->debug("<<PING<<");
                            SendPong();
                            return;
                        } else if (groupHeartBeat.getHeartBeatType() == riaps::distrcoord::HeartBeatType::PONG) {
                            //_logger->debug("<<PONG<<");
                            return;
                        }
                    } else if (internal.hasLeaderElection()){
                        auto msgLeader = internal.getLeaderElection();
                        _groupLeader->Update(msgLeader);
                        return;
                    } else if (internal.hasMessageToLeader()){
                        auto msgLeader = internal.getMessageToLeader();
                        if (GetLeaderId() != GetParentComponent()->GetCompUuid()) return;

                        zframe_t* leaderMsgFrame = zmsg_pop(msg);
                        if (!leaderMsgFrame) return;
                        auto leaderMsgPtr = std::shared_ptr<zframe_t>(leaderMsgFrame, [](zframe_t* z){zframe_destroy(&z);});
                        capnp::FlatArrayMessageReader capnpLeaderMsg(nullptr);
                        (*leaderMsgFrame) >> capnpLeaderMsg;

                        _parentComponent->OnMessageToLeader(m_groupId, capnpLeaderMsg);

                        return;
                    } else if (internal.hasConsensus()) {
                        auto msgCons = internal.getConsensus();
                       // m_logger->debug("DC message arrived from {}", msgDistCoord.getSourceComponentId().cStr());

//                        // The current component is the leader
                        if (GetLeaderId() == GetParentComponentId()) {
                            //m_logger->debug("DC message arrived and this component is the leader");

//                            // Propose arrived to the leader. Leader forwards it to every groupmember.
                            // TODO: We may not need the forwarding step, since everybody got the message
                            if (msgCons.hasProposeToLeader()){
                                auto msgPropose = msgCons.getProposeToLeader();

                                // Value is proposed
                                if (msgCons.getVoteType() == riaps::distrcoord::Consensus::VoteType::VALUE){
                                    zframe_t* proposeFrame;
                                    proposeFrame = zmsg_pop(msg);
                                    _logger->info("Message proposed to the leader, proposeId: {}", msgPropose.getProposeId().cStr());
                                    _groupLeader->OnProposeFromClient(msgPropose, &proposeFrame);
                                    if (proposeFrame!= nullptr)
                                        zframe_destroy(&proposeFrame);
                                }

                                    // Action is proposed to the leader
                                else if (msgCons.getVoteType() == riaps::distrcoord::Consensus::VoteType::ACTION) {
                                    auto msgTsca = msgCons.getTsyncCoordA();
                                    _logger->info("Action proposed to the leader, proposeId: {}, actionId: {}",
                                                  msgPropose.getProposeId().cStr(),
                                                  msgTsca.getActionId().cStr());
                                    _groupLeader->OnActionProposeFromClient(msgPropose,msgTsca);
                                }

//                            // Vote arrived, count the votes and announce the results (if any)
                            } else if (msgCons.hasVote()){
                                //m_logger->info("Vote arrived to the leader");
                                auto msgVote = msgCons.getVote();
                                _groupLeader->OnVote(msgVote, msgCons.getSourceComponentId());
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
                                    _parentComponent->OnPropose(m_groupId, msgPropose.getProposeId(), *reader);
                                } else if (msgCons.getVoteType() == riaps::distrcoord::Consensus::VoteType::ACTION) {
                                    timespec t{
                                            msgCons.getTsyncCoordA().getTime().getTvSec(),
                                            msgCons.getTsyncCoordA().getTime().getTvNsec()
                                    };

                                    _logger->error_if(t.tv_sec!=msgCons.getTsyncCoordA().getTime().getTvSec(),"tv_sec!=TvSec");
                                    _logger->error_if(t.tv_nsec!=msgCons.getTsyncCoordA().getTime().getTvNsec(),"tv_nsec!=TvNsec");

                                    _parentComponent->OnActionPropose(m_groupId,
                                                                      msgPropose.getProposeId(),
                                                                      msgCons.getTsyncCoordA().getActionId(),
                                                                      t);
                                }
                            } else if (msgCons.hasAnnounce()) {
                                auto msgAnnounce = msgCons.getAnnounce();
                                _parentComponent->OnAnnounce(m_groupId,
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

        bool Group::SendVote(const std::string &proposeId, bool accept) {
            capnp::MallocMessageBuilder builder;
            auto msgInt = builder.initRoot<riaps::distrcoord::GroupInternals>();
            auto msgCons = msgInt.initConsensus();
            msgCons.setSourceComponentId(GetParentComponent()->GetCompUuid());
            auto msgVote = msgCons.initVote();
            msgVote.setProposeId(proposeId);
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
            if (_groupPoller != nullptr)
                zpoller_destroy(&_groupPoller);
        }
    }
}
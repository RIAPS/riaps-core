//
// Created by istvan on 10/3/17.
//

#include <groups/r_group.h>

#include <capnp/common.h>

namespace riaps{
    namespace groups{

        using namespace riaps;

        bool GroupId::operator<(const GroupId &other) const {
            if (groupTypeId == other.groupTypeId){
                return groupName < other.groupName;
            }
            return groupTypeId<other.groupTypeId;
        }

        Group::Group(const GroupId &groupId, const std::string componentId) :
                _groupId(groupId),
                _componentId(componentId),
                _groupPubPort(nullptr),
                _groupSubPort(nullptr),
                _lastFrame(nullptr),
                _lastPing(0),
                _pingPeriod(10*1000) {


        }

        bool Group::InitGroup() {

            // If the groupid doesn't exist, just skip the initialization and return false
            auto groupTypeConf = ::riaps::Actor::GetRunningActor().GetGroupType(_groupId.groupTypeId);
            if (groupTypeConf == nullptr)
                return false;

            _groupTypeConf = *groupTypeConf;

            // Default port for the group. Reserved for RIAPS internal communication protocols
            _group_port_pub internalPubConfig;
            internalPubConfig.messageType = INTERNAL_MESSAGETYPE;
            internalPubConfig.isLocal     = false;
            internalPubConfig.portName    = INTERNAL_PUB_NAME;

            std::vector<GroupService> initializedServices;

            _groupPubPort = std::shared_ptr<ports::GroupPublisherPort>(new ports::GroupPublisherPort(internalPubConfig));
            initializedServices.push_back(_groupPubPort->GetGroupService());
            _groupPorts[_groupPubPort->GetSocket()] = _groupPubPort;


            _group_port_sub internalSubConfig;
            internalSubConfig.messageType = INTERNAL_MESSAGETYPE;
            internalSubConfig.isLocal     = false;
            internalSubConfig.portName    = INTERNAL_SUB_NAME;

            _groupSubPort = std::shared_ptr<ports::GroupSubscriberPort>(new ports::GroupSubscriberPort(internalSubConfig));
            _groupPorts[_groupSubPort->GetSocket()] = _groupSubPort;



            // Initialize the zpoller and add the group sub port
            _groupPoller = zpoller_new(const_cast<zsock_t*>(_groupSubPort->GetSocket()), nullptr);

            // Initialize user defined publishers
            for(auto& portDeclaration : _groupTypeConf.groupTypePorts.pubs){
                auto newPubPort = std::shared_ptr<ports::GroupPublisherPort>(new ports::GroupPublisherPort(portDeclaration));
                initializedServices.push_back(newPubPort->GetGroupService());
                _groupPorts[newPubPort->GetSocket()]=std::move(newPubPort);

            }

            // Initialize user defined subscribers
            for(auto& portDeclaration : _groupTypeConf.groupTypePorts.subs){
                auto newSubPort = std::shared_ptr<ports::GroupSubscriberPort>(new ports::GroupSubscriberPort(portDeclaration));
                zpoller_add(_groupPoller, const_cast<zsock_t*>(newSubPort->GetSocket()));
                _groupPorts[newSubPort->GetSocket()] = std::move(newSubPort);

            }

            // Register all of the publishers
            return joinGroup(riaps::Actor::GetRunningActor().GetApplicationName(),
                             _componentId,
                             _groupId,
                             initializedServices);
        }

        bool Group::SendMessage(capnp::MallocMessageBuilder &message, const std::string &portName) {

            for (auto it = _groupPorts.begin(); it!=_groupPorts.end(); it++){
                auto currentPort = it->second->AsGroupPublishPort();
                if (currentPort == nullptr) continue;
                if (currentPort->GetConfig()->portName != portName) continue;

                return currentPort->Send(message);

            }

            return false;
        }

        std::shared_ptr<std::vector<std::string>> Group::GetKnownComponents() {
            std::shared_ptr<std::vector<std::string>> result(new std::vector<std::string>());

            std::transform(_knownNodes.begin(),
                           _knownNodes.end(),
                           std::back_inserter(*result),
                           [](const std::pair<std::string, int64_t >& p) -> std::string {
                               return p.first;
                           });

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

        void Group::SendHeartBeat(riaps::distrcoord::HeartBeatType type) {
            int64_t currentTime = zclock_mono();

            if (_lastPing == 0)
                _lastPing = currentTime;

            // If outdated, send ping
            if ((currentTime - _lastPing) > _pingPeriod){
                capnp::MallocMessageBuilder builder;
                auto heartbeat = builder.initRoot<riaps::distrcoord::GroupHeartBeat>();

                heartbeat.setHeartBeatType(type);
                heartbeat.setSourceComponentId(this->_componentId);

                if (_groupPubPort->Send(builder)) {
                    _lastPing = currentTime;
                    std::cout << ">>PING>>" << std::endl;
                }
                else std::cout << "PING failed" << std::endl;
            }
        }

        uint16_t Group::GetMemberCount(uint16_t timeout) const {
            auto result =0;

            int64_t now  = zclock_mono();
            int64_t from = now - timeout;

            for (auto& pair : _knownNodes) {
                if (pair.second >= from) result++;
            }

            return result;
        }

        ports::GroupSubscriberPort* Group::FetchNextMessage(std::unique_ptr<capnp::FlatArrayMessageReader>& messageReader) {
            void* which = zpoller_wait(_groupPoller, 10);
            if (which == nullptr) return nullptr;

            // Look for the port

            ports::PortBase* currentPort = nullptr;
            try{
                currentPort = _groupPorts.at(static_cast<zsock_t*>(which)).get();
            } catch (std::out_of_range& e){
                return nullptr;
            }

            riaps::ports::GroupSubscriberPort* subscriberPort = currentPort->AsGroupSubscriberPort();

            // If the port is not a subscriber port (in theory this is impossible)
            if (subscriberPort == nullptr)  return nullptr;

            // Delete the previous message if there is any
            if (_lastFrame != nullptr) {
                zframe_destroy(&_lastFrame);
                _lastFrame = nullptr;
            };

            zmsg_t* msg = zmsg_recv(const_cast<zsock_t*>(subscriberPort->GetSocket()));

            if (msg) {

                // _lstFrame owns the data. Must be preserved until the next message
                _lastFrame  = zmsg_pop(msg);
                size_t size = zframe_size(_lastFrame);
                byte *data  = zframe_data(_lastFrame);

                kj::ArrayPtr<const capnp::word> capnp_data(reinterpret_cast<const capnp::word *>(data), size / sizeof(capnp::word));

                messageReader.reset(new capnp::FlatArrayMessageReader(capnp_data));

                // Internal port, handle it here and don't send any notifications to the caller
                if (subscriberPort == _groupSubPort.get()){
                    capnp::FlatArrayMessageReader reader(capnp_data);
                    auto groupHeartBeat = reader.getRoot<riaps::distrcoord::GroupHeartBeat>();
                    if (groupHeartBeat.getHeartBeatType() == riaps::distrcoord::HeartBeatType::PING){
                        std::cout << "<<PING<<" << std::endl;
                        _lastPing = zclock_mono();
                        SendHeartBeat(riaps::distrcoord::HeartBeatType::PONG);
                        std::cout << ">>PONG>>" << std::endl;
                        _knownNodes[groupHeartBeat.getSourceComponentId()] = _lastPing;
                    } else if (groupHeartBeat.getHeartBeatType() == riaps::distrcoord::HeartBeatType::PONG){
                        std::cout << "<<PONG<<" <<std::endl;
                        _knownNodes[groupHeartBeat.getSourceComponentId()] = zclock_mono();
                    }
                }
            }

            zmsg_destroy(&msg);
            return subscriberPort;
        }

        Group::~Group() {

        }
    }
}
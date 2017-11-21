//
// Created by istvan on 10/3/17.
//

#include <groups/r_group.h>

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

        Group::Group(const GroupId &groupId, const ComponentBase* parentComponent) :
                _groupId(groupId),
                _parentComponent(parentComponent),
                _groupPubPort(nullptr),
                _groupSubPort(nullptr),
                _lastFrame(nullptr),
                _lastPingSent(0),
                _pingPeriod(PING_BASE_PERIOD) {
            _pingCounter = 0;
            _logger = spd::get(parentComponent->GetConfig().component_name);
            rndDistribution = std::uniform_int_distribution<int>(1000, 5000);
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

            _groupPubPort = std::shared_ptr<ports::GroupPublisherPort>(new ports::GroupPublisherPort(internalPubConfig, _parentComponent));
            initializedServices.push_back(_groupPubPort->GetGroupService());
            _groupPorts[_groupPubPort->GetSocket()] = _groupPubPort;


            _group_port_sub internalSubConfig;
            internalSubConfig.messageType = INTERNAL_MESSAGETYPE;
            internalSubConfig.isLocal     = false;
            internalSubConfig.portName    = INTERNAL_SUB_NAME;

            _groupSubPort = std::shared_ptr<ports::GroupSubscriberPort>(new ports::GroupSubscriberPort(internalSubConfig, _parentComponent));
            _groupPorts[_groupSubPort->GetSocket()] = _groupSubPort;



            // Initialize the zpoller and add the group sub port
            _groupPoller = zpoller_new(const_cast<zsock_t*>(_groupSubPort->GetSocket()), nullptr);

            // Initialize user defined publishers
            for(auto& portDeclaration : _groupTypeConf.groupTypePorts.pubs){
                auto newPubPort = std::shared_ptr<ports::GroupPublisherPort>(new ports::GroupPublisherPort(portDeclaration, _parentComponent));
                initializedServices.push_back(newPubPort->GetGroupService());
                _groupPorts[newPubPort->GetSocket()]=std::move(newPubPort);

            }

            // Initialize user defined subscribers
            for(auto& portDeclaration : _groupTypeConf.groupTypePorts.subs){
                auto newSubPort = std::shared_ptr<ports::GroupSubscriberPort>(new ports::GroupSubscriberPort(portDeclaration, _parentComponent));
                zpoller_add(_groupPoller, const_cast<zsock_t*>(newSubPort->GetSocket()));
                _groupPorts[newSubPort->GetSocket()] = std::move(newSubPort);

            }

            // Register all of the publishers
            return joinGroup(riaps::Actor::GetRunningActor().GetApplicationName(),
                             _parentComponent->GetCompUuid(),
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

        bool Group::SendPing() {
            _logger->debug(">>PING>>");
            _lastPingSent = zclock_mono();
            return SendHeartBeat(dc::HeartBeatType::PING);
        }

        bool Group::SendPong() {
            _logger->debug(">>PONG>>");
            return SendHeartBeat(dc::HeartBeatType::PONG);
        }

        bool Group::SendPingWithPeriod() {
            int64_t currentTime = zclock_mono();

            if (_lastPingSent == 0)
                _lastPingSent = currentTime;

            // If the ping period has been reached
            if ((currentTime - _lastPingSent) > _pingPeriod){
                return SendPing();
            }
            return false;
        }


        bool Group::SendHeartBeat(riaps::distrcoord::HeartBeatType type) {
            capnp::MallocMessageBuilder builder;
            auto heartbeat = builder.initRoot<riaps::distrcoord::GroupHeartBeat>();

            heartbeat.setHeartBeatType(type);
            heartbeat.setSourceComponentId(this->_parentComponent->GetCompUuid());

            return _groupPubPort->Send(builder);
        }

        uint16_t Group::GetMemberCount(uint16_t timeout) const {
            auto result =0;

            _logger->warn_if(timeout<_pingPeriod,"Timeout of group members in count() are less then the ping period.");

            int64_t now  = zclock_mono();
            int64_t from = now - timeout;

            for (auto& pair : _knownNodes) {
                if (pair.second >= from) result++;
            }

            return result;
        }

        ports::GroupSubscriberPort* Group::FetchNextMessage(std::shared_ptr<capnp::FlatArrayMessageReader>& messageReader) {
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
                        _logger->debug("<<PING<<");
                        SendPong();
                        //SendHeartBeat(riaps::distrcoord::HeartBeatType::PONG, groupHeartBeat.getSequenceNumber());
                        //_logger->debug(">>PONG>>");
                        return nullptr;
                    } else if (groupHeartBeat.getHeartBeatType() == riaps::distrcoord::HeartBeatType::PONG){
                        _logger->debug("<<PONG<<");
                        _knownNodes[groupHeartBeat.getSourceComponentId()] = zclock_mono();
                        return nullptr;
                    }

//                    _logger->debug("[In] Internal message: {}", (uint16_t)groupHeartBeat.getHeartBeatType());
//                    if (groupHeartBeat.getHeartBeatType() == riaps::distrcoord::HeartBeatType::PING){
//                        //_logger->debug("<<PING<<");
//                        _lastPingSent = zclock_mono();
//                        SendHeartBeat(riaps::distrcoord::HeartBeatType::PONG);
//                        //_logger->debug(">>PONG>>");
//                        _knownNodes[groupHeartBeat.getSourceComponentId()] = _lastPingSent;
//                        _logger->debug("[Out] Internal message: {}", "PONG");
//                        // Return null, no handler for the inherited class.
//                        return nullptr;
//                    } else if (groupHeartBeat.getHeartBeatType() == riaps::distrcoord::HeartBeatType::PONG){
//                        //_logger->debug("<<PONG<<");
//                        _knownNodes[groupHeartBeat.getSourceComponentId()] = zclock_mono();
//
//                        // Return nullptr, no handler will be called for the developer
//                        return nullptr;
//                    }
                }
            }

            zmsg_destroy(&msg);
            return subscriberPort;
        }

        Group::~Group() {
            if (_lastFrame!= nullptr){
                zframe_destroy(&_lastFrame);
                _lastFrame=nullptr;
            }
        }
    }
}
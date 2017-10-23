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
                _lastFrame(nullptr){



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
            _groupPorts[_groupSubPort->GetSocket()] = _groupPubPort;

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

            auto subscriberPort = currentPort->AsGroupSubscriberPort();

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
            }

            zmsg_destroy(&msg);
        }

        Group::~Group() {

        }
    }
}
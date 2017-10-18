//
// Created by istvan on 10/3/17.
//

#include <groups/r_group.h>

namespace riaps{
    namespace groups{

        using namespace riaps;

        bool GroupId::operator<(const GroupId &other) const {
            if (groupTypeId == other.groupTypeId){
                return groupName < other.groupName;
            }
            return groupTypeId<other.groupTypeId;
        }

        Group::Group(const GroupId &groupId, const std::string& componentId) :
                _groupId(groupId),
                _componentId(componentId),
                _groupPubPort(nullptr),
                _groupSubPort(nullptr) {

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

            _groupPubPort = std::unique_ptr<ports::GroupPublisherPort>(new ports::GroupPublisherPort(internalPubConfig));
            initializedServices.push_back(_groupPubPort->GetGroupService());


            _group_port_sub internalSubConfig;
            internalSubConfig.messageType = INTERNAL_MESSAGETYPE;
            internalSubConfig.isLocal     = false;
            internalSubConfig.portName    = INTERNAL_SUB_NAME;

            _groupSubPort = std::unique_ptr<ports::GroupSubscriberPort>(new ports::GroupSubscriberPort(internalSubConfig));



            // Initialize user defined publishers
            for(auto& portDeclaration : _groupTypeConf.groupTypePorts.pubs){
                auto newPubPort = std::unique_ptr<ports::GroupPublisherPort>(new ports::GroupPublisherPort(portDeclaration));
                initializedServices.push_back(newPubPort->GetGroupService());
                _groupPorts.push_back(std::move(newPubPort));

            }

            // Initialize user defined subscribers
            for(auto& portDeclaration : _groupTypeConf.groupTypePorts.subs){
                auto newSubPort = std::unique_ptr<ports::GroupSubscriberPort>(new ports::GroupSubscriberPort(portDeclaration));
                _groupPorts.push_back(std::move(newSubPort));
            }

            // Register all of the publishers
            return joinGroup(riaps::Actor::GetRunningActor().GetApplicationName(),
                             _componentId,
                             _groupId,
                             initializedServices);
        }

        bool Group::SendMessage(capnp::MallocMessageBuilder &message, const std::string &portName) {

            for (auto it = _groupPorts.begin(); it!=_groupPorts.end(); it++){
                auto currentPort = (*it)->AsGroupPublishPort();
                if (currentPort == nullptr) continue;
                if (currentPort->GetConfig()->portName != portName) continue;
                
                currentPort->Send(message);

            }
        }

        void Group::ConnectToNewServices(riaps::discovery::GroupUpdate::Reader &msgGroupUpdate) {
            for (int i =0; i<msgGroupUpdate.getServices().size(); i++){
                std::string messageType = msgGroupUpdate.getServices()[i].getMessageType().cStr();

                // RIAPS port
                if (messageType == INTERNAL_MESSAGETYPE){
                    std::string address = msgGroupUpdate.getServices()[i].getAddress().cStr();
                    address = "tcp://" + address;
                    _groupSubPort->ConnectToPublihser(address);
                    continue;
                }

                for (auto& groupPort : _groupPorts){
                    auto subscriberPort = groupPort->AsGroupSubscriberPort();
                    if (subscriberPort == nullptr) continue;
                    if (subscriberPort->GetConfig()->messageType == messageType){
                        std::string address = msgGroupUpdate.getServices()[i].getAddress().cStr();
                        address = "tcp://" + address;
                        subscriberPort->ConnectToPublihser(address);
                    }
                }
            }
        }

        Group::~Group() {

        }
    }
}
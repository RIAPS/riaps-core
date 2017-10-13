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
            auto groupTypeConf = Actor::GetRunningActor().GetGroupType(_groupId.groupTypeId);
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



            // Initialize user defined ports
            for(auto& portDeclaration : _groupTypeConf.groupTypePorts.pubs){
                auto newPort = std::unique_ptr<ports::GroupPublisherPort>(new ports::GroupPublisherPort(portDeclaration));
                initializedServices.push_back(newPort->GetGroupService());
                _groupPorts.push_back(std::move(newPort));
            }

            // Register all of the publishers
            return joinGroup(riaps::Actor::GetRunningActor().GetApplicationName(),
                             _componentId,
                             _groupId,
                             initializedServices);
        }

        void Group::ConnectToNewServices(riaps::discovery::GroupUpdate::Reader &msgGroupUpdate) {
            for (int i =0; i<msgGroupUpdate.getServices().size(); i++){
                std::string messageType = msgGroupUpdate.getServices()[i].getMessageType().cStr();
                for (auto& groupPort : _groupPorts){
                    auto subscriberPort = groupPort->AsSubscribePort();
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
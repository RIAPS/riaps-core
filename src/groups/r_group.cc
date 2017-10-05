//
// Created by istvan on 10/3/17.
//

#include <groups/r_group.h>
#include <componentmodel/r_pubportgroup.h>



namespace riaps{
    namespace groups{

        using namespace riaps;

        bool GroupId::operator<(const GroupId &other) const {
            if (groupTypeId == other.groupTypeId){
                return groupName < other.groupName;
            }
            return groupTypeId<other.groupTypeId;
        }

        Group::Group(const GroupId &groupId) :
                _groupId(groupId),
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
                initializedServices.push_back(_groupPubPort->GetGroupService());
                _groupPorts.push_back(std::move(newPort));
            }

            // Register all of the publishers
            joinGroup(riaps::Actor::GetRunningActor().GetApplicationName(),
                      _groupId,
                      initializedServices);
        }

        Group::~Group() {

        }
    }
}
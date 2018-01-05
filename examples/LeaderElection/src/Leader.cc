//
// Created by istvan on 11/11/16.
//

#include "Leader.h"
#include <capnp/serialize.h>
#include <capnp/message.h>

namespace leaderelection {
    namespace components {

        Leader::Leader(_component_conf &config, riaps::Actor &actor) : LeaderBase(config, actor) {
            //PrintParameters();
            _hasJoined = false;
            SetDebugLevel(_logger, spdlog::level::level_enum::debug);
        }

        void Leader::OnClock(riaps::ports::PortBase *port) {
            //int64_t time = zclock_mono();

            if (!_hasJoined) {
                bool rc = this->JoinToGroup({"BackupGroup", "Korte"});
                if (rc) {
                    _logger->debug("Successfully joined to group");
                    _hasJoined = true;
                } else {
                    _logger->debug("Failed to join");
                }
            } else {
                riaps::groups::GroupId gid{"BackupGroup", "Korte"};
                auto ct = GetGroupMemberCount(gid, 5000);
                std::string ld = GetLeaderId(gid);
                _logger->debug("{}, ({})", ct, ld);

            }
        }



        void Leader::OnGroupMessage(const riaps::groups::GroupId &groupId,
                                         capnp::FlatArrayMessageReader &capnpreader, riaps::ports::PortBase *port) {

        }


        bool Leader::SendGroupMessage(riaps::groups::GroupId &groupId, capnp::MallocMessageBuilder &messageBuilder,
                                           const std::string &portName) {
            return true;
        }

        Leader::~Leader() {

        }
    }
}

riaps::ComponentBase* create_component(_component_conf& config, riaps::Actor& actor){
    auto result = new leaderelection::components::Leader(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
//
// Created by istvan on 11/11/16.
//

#include "Consensus.h"
#include <capnp/serialize.h>
#include <capnp/message.h>
#include <fstream>

namespace dc {
    namespace components {

        Consensus::Consensus(_component_conf &config, riaps::Actor &actor) : ConsensusBase(config, actor) {
            _hasJoined = false;
            SetDebugLevel(_logger, spdlog::level::level_enum::debug);
            _generator = std::mt19937(_rd());
            _distr     = std::uniform_int_distribution<int>(1, 2);

            std::ofstream o;
            o.open("dcoordvote.txt");
            o << _distr(_generator) << std::endl;
            o.close();
        }

        void Consensus::OnClock(riaps::ports::PortBase *port) {
            //int64_t time = zclock_mono();

            if (!_hasJoined) {
                bool rc = this->JoinToGroup({GROUP_TYPE_GROUP1, "Korte"});
                if (rc) {
                    _logger->debug("Successfully joined to group");
                    _hasJoined = true;
                } else {
                    _logger->debug("Failed to join");
                }
            } else {
                riaps::groups::GroupId gid{GROUP_TYPE_GROUP1, "Korte"};


                // If the component is not the leader, then propose()
                if (GetLeaderId(gid)!="" && GetLeaderId(gid)!=GetCompUuid()) {
                    // The component already joined, read the file and lets vote about the content of the file
                    std::ifstream f;
                    f.open("dcoordvote.txt");
                    std::string line;
                    f >> line;

                    capnp::MallocMessageBuilder builder;
                    auto msgDc = builder.initRoot<dc::messages::AgreeOnThis>();
                    msgDc.setValue(line);
                    std::string proposeId = SendPropose(gid, builder);
                    std::string leaderId = GetLeaderId(gid);
                    _logger->info("Proposed value: {}", line);
                }
            }
        }

        void Consensus::OnPropose(riaps::groups::GroupId &groupId, const std::string &proposeId,
                                   capnp::FlatArrayMessageReader &message) {
            auto msg = message.getRoot<dc::messages::AgreeOnThis>();
            std::ifstream f;
            f.open("dcoordvote.txt");
            std::string line;
            f >> line;

            if (line == msg.getValue().cStr()){
                bool rc = SendVote(groupId, proposeId, true);
                _logger->debug_if(rc,"Vote - ACCEPT {}", proposeId);
                _logger->error_if(!rc, "Vote - ACCEPT Failed");
            } else{
                SendVote(groupId, proposeId, false);
                _logger->debug("Vote - REJECT {}", proposeId);
            }

        }

        void Consensus::OnAnnounce(const riaps::groups::GroupId &groupId, const std::string &proposeId,
                                    bool accepted) {
            _logger->info("Announce, Propose Id: {} is {}", proposeId, accepted?"accepted":"rejected");
        }

        void Consensus::OnGroupMessage(const riaps::groups::GroupId &groupId,
                                         capnp::FlatArrayMessageReader &capnpreader, riaps::ports::PortBase *port) {

        }


        bool Consensus::SendGroupMessage(riaps::groups::GroupId &groupId, capnp::MallocMessageBuilder &messageBuilder,
                                           const std::string &portName) {
            return true;
        }


        Consensus::~Consensus() {

        }
    }
}

riaps::ComponentBase* create_component(_component_conf& config, riaps::Actor& actor){
    auto result = new dc::components::Consensus(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
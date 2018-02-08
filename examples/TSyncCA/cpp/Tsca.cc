#include "include/Tsca.h"

namespace tsyncca {
   namespace components {
      
      Tsca::Tsca(_component_conf &config, riaps::Actor &actor) :
      TscaBase(config, actor), m_hasJoined(false) {
      }
      
      void Tsca::OnClock(riaps::ports::PortBase *port) {
          if (!m_hasJoined) {
              bool rc = this->JoinToGroup({GROUP_TYPE_GROUPA, "Korte"});
              if (rc) {
                  _logger->debug("Successfully joined to group");
                  m_hasJoined = true;
              } else {
                  _logger->debug("Failed to join");
              }
          } else {
              riaps::groups::GroupId gid{GROUP_TYPE_GROUPA, "Korte"};
              auto ct = GetGroupMemberCount(gid, 5000);

              std::string ld = GetLeaderId(gid);
              //_logger->debug("{}, ({})", ct, ld);
              if (ld != GetCompUuid()){
                  capnp::MallocMessageBuilder builder;
                  auto msg = builder.initRoot<leaderelection::messages::LeaderMessage>();
                  msg.setMsg("Hello");
                  bool rc = SendMessageToLeader(gid, builder);
                  _logger->error_if(!rc,"Coldn't sent message to the leader");
                  _logger->info_if(rc,"Message sent to the leader {}->{}", GetCompUuid(),GetLeaderId(gid));
              }
          }
      }
      
      void Tsca::OnGroupMessage(const riaps::groups::GroupId& groupId,
      capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){
         
      }
      
      Tsca::~Tsca() {
         
      }
   }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
   auto result = new tsyncca::components::Tsca(config, actor);
   return result;
}

void destroy_component(riaps::ComponentBase *comp) {
   delete comp;
}

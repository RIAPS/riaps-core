#include <CompThree.h>

namespace leaderwithtree {
   namespace components {
      
      CompThree::CompThree(_component_conf &config, riaps::Actor &actor) :
      CompThreeBase(config, actor), m_joinedToA(false) {
          _logger->set_pattern("[%n] %v");
      }
      
      void CompThree::OnClock(riaps::ports::PortBase *port) {
         if (!m_joinedToA){
             _logger->info("Component joins to {}:{}", groupIdA.groupTypeId, groupIdA.groupName);
            auto joined = JoinGroup(groupIdA);
            if (joined){
                m_joinedToA = true;
            }
            _logger->error_if(!joined, "Couldn't join to group {}:{}", groupIdA.groupTypeId, groupIdA.groupName);
         }

         _logger->info("The leader is: {}", GetLeaderId(groupIdA));
      }
      
      void CompThree::OnGroupMessage(const riaps::groups::GroupId& groupId,
      capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){
      }
      
      CompThree::~CompThree() {
         
      }
   }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
   auto result = new leaderwithtree::components::CompThree(config, actor);
   return result;
}

void destroy_component(riaps::ComponentBase *comp) {
   delete comp;
}

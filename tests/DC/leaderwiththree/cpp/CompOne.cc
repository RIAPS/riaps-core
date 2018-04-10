#include <CompOne.h>

namespace leaderwithtree {
   namespace components {
      
      CompOne::CompOne(_component_conf &config, riaps::Actor &actor) :
      CompOneBase(config, actor), m_joinedToA(false) {
          _logger->set_pattern("[%n] %v");
      }
      
      void CompOne::OnClock(riaps::ports::PortBase *port) {
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
    
      
      void CompOne::OnGroupMessage(const riaps::groups::GroupId& groupId, capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){
      }
      
      CompOne::~CompOne() {
         
      }
   }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
   auto result = new leaderwithtree::components::CompOne(config, actor);
   return result;
}

void destroy_component(riaps::ComponentBase *comp) {
   delete comp;
}

#include <CompTwo.h>

namespace groupmsgtest {
   namespace components {
      
      CompTwo::CompTwo(_component_conf &config, riaps::Actor &actor) :
      CompTwoBase(config, actor), m_joinedToA(false), m_joinedToB(false) {
          _logger->set_pattern("[%n] %v");
      }
      
      void CompTwo::OnClock(riaps::ports::PortBase *port) {
         if (!m_joinedToA){
             _logger->info("Component joins to {}:{}", groupIdA.groupTypeId, groupIdA.groupName);
            auto joined = JoinGroup(groupIdA);
            if (joined){
                m_joinedToA = true;
            }
            _logger->error_if(!joined, "Couldn't join to group {}:{}", groupIdA.groupTypeId, groupIdA.groupName);
         }

         if (!m_joinedToB){
             _logger->info("Component joins to {}:{}", groupIdB.groupTypeId, groupIdB.groupName);
            auto joined = JoinGroup(groupIdB);
            if (joined){
                m_joinedToB = true;
            }
            _logger->error_if(!joined, "Couldn't join to group {}:{}", groupIdB.groupTypeId, groupIdB.groupName);
         }
      }
      
      void CompTwo::OnGroupMessage(const riaps::groups::GroupId& groupId,
      capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){

          logGroupMessage(_logger, __FUNCTION__, groupId, capnpreader.getRoot<MessageType>().getMsg());
      }
      
      CompTwo::~CompTwo() {
         
      }
   }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
   auto result = new groupmsgtest::components::CompTwo(config, actor);
   return result;
}

void destroy_component(riaps::ComponentBase *comp) {
   delete comp;
}

#include <CompTwo.h>

namespace groupmsgtest {
   namespace components {
      
      CompTwo::CompTwo(_component_conf &config, riaps::Actor &actor) :
      CompTwoBase(config, actor), m_joinedToA(false), m_joinedToB(false) {
          _logger->set_pattern("[%n] %v");
      }
      
      void CompTwo::OnClock(riaps::ports::PortBase *port) {
         if (!m_joinedToA){
             _logger->info("Component joins {}:{}", groupIdA.groupTypeId, groupIdA.groupName);
            auto joined = JoinGroup(groupIdA);
            if (joined){
                m_joinedToA = true;
                m_timeout = Timeout<std::ratio<1>>(duration<int, std::ratio<1>>(20)); // 20 seconds
            }
            _logger->error_if(!joined, "Couldn't join group {}:{}", groupIdA.groupTypeId, groupIdA.groupName);
         } else if (m_timeout.IsTimeout()){
             // If timeout -> leave the group
             _logger->info("leaves the group: ({}, {})", groupIdA.groupTypeId, groupIdA.groupName);
             LeaveGroup(groupIdA);
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

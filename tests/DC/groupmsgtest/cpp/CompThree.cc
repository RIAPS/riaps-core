#include <CompThree.h>

namespace groupmsgtest {
   namespace components {
      
      CompThree::CompThree(_component_conf &config, riaps::Actor &actor) :
      CompThreeBase(config, actor) {
          _logger->set_pattern("[%n] %v");
      }
      
      void CompThree::OnClock(riaps::ports::PortBase *port) {
         if (!m_joinedToB){
             _logger->info("Component joins to {}:{}", groupIdB.groupTypeId, groupIdB.groupName);
            auto joined = JoinGroup(groupIdB);
            if (joined){
                m_joinedToB = true;
            }
            _logger->error_if(!joined, "Couldn't join to group {}:{}", groupIdB.groupTypeId, groupIdB.groupName);
         }
         else {
             capnp::MallocMessageBuilder builder;
             auto grpMsg = builder.initRoot<MessageType>();
             auto msgContent = fmt::format("{}", GetComponentName());
             grpMsg.setMsg(msgContent);
             _logger->error_if(!SendGroupMessage(groupIdB, builder), "{} couldn't send groupmessage", GetComponentName());
         }
      }
      
      void CompThree::OnGroupMessage(const riaps::groups::GroupId& groupId,
      capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){
          logGroupMessage(_logger, __FUNCTION__, groupId, capnpreader.getRoot<MessageType>().getMsg());
      }
      
      CompThree::~CompThree() {
         
      }
   }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
   auto result = new groupmsgtest::components::CompThree(config, actor);
   return result;
}

void destroy_component(riaps::ComponentBase *comp) {
   delete comp;
}

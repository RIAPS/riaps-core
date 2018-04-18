#include <CompOne.h>

namespace leadermessage {
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
         } else {
             if (!IsLeader(groupIdA)) {
                 capnp::MallocMessageBuilder builder;
                 auto msg = builder.initRoot<MessageType>();
                 msg.setMsg(fmt::format("{}", GetComponentName()));
                 SendMessageToLeader(groupIdA, builder);
             }
         }
      }

      void CompOne::OnMessageToLeader(const riaps::groups::GroupId &groupId, capnp::FlatArrayMessageReader &message) {
          _logger->info("Messaged arrived from component {}", message.getRoot<MessageType>().getMsg().cStr());
          capnp::MallocMessageBuilder builder;
          auto msg = builder.initRoot<MessageType>();
          msg.setMsg(GetCompUuid());

      }

      void CompOne::OnMessageFromLeader(const riaps::groups::GroupId &groupId, capnp::FlatArrayMessageReader &message) {
          _logger->info("Messaged arrived from leader {}", message.getRoot<MessageType>().getMsg().cStr());
      }
    
      
      void CompOne::OnGroupMessage(const riaps::groups::GroupId& groupId, capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){
      }
      
      CompOne::~CompOne() {
         
      }
   }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
   auto result = new leadermessage::components::CompOne(config, actor);
   return result;
}

void destroy_component(riaps::ComponentBase *comp) {
   delete comp;
}

#include <CompTwo.h>

namespace groupmsgtest {
   namespace components {
      
      CompTwo::CompTwo(_component_conf &config, riaps::Actor &actor) :
      CompTwoBase(config, actor) {
      }
      
      void CompTwo::OnClock(riaps::ports::PortBase *port) {
         std::cout << "CompTwo::OnClock(): " << port->GetPortName() << std::endl;
      }
      
      void CompTwo::OnGroupMessage(const riaps::groups::GroupId& groupId,
      capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){
         
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

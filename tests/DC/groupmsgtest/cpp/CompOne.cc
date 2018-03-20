#include <CompOne.h>

namespace groupmsgtest {
   namespace components {
      
      CompOne::CompOne(_component_conf &config, riaps::Actor &actor) :
      CompOneBase(config, actor) {
      }
      
      void CompOne::OnClock(riaps::ports::PortBase *port) {
         std::cout << "CompOne::OnClock(): " << port->GetPortName() << std::endl;
      }
      
      void CompOne::OnGroupMessage(const riaps::groups::GroupId& groupId,
      capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){
         
      }
      
      CompOne::~CompOne() {
         
      }
   }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
   auto result = new groupmsgtest::components::CompOne(config, actor);
   return result;
}

void destroy_component(riaps::ComponentBase *comp) {
   delete comp;
}

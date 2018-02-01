#include <ATimer.h>

namespace adaptivetimer {
   namespace components {
      
      ATimer::ATimer(_component_conf &config, riaps::Actor &actor) :
      ATimerBase(config, actor) {
      }
      
      void ATimer::OnClock(riaps::ports::PortBase *port) {
         std::cout << "ATimer::OnClock(): " << port->GetPortName() << std::endl;
      }
      
      void ATimer::OnGroupMessage(const riaps::groups::GroupId& groupId,
      capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){
         
      }
      
      ATimer::~ATimer() {
         
      }
   }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
   auto result = new adaptivetimer::components::ATimer(config, actor);
   return result;
}

void destroy_component(riaps::ComponentBase *comp) {
   delete comp;
}

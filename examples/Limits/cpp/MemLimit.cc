#include <MemLimit.h>

namespace limits {
   namespace components {
      
      MemLimit::MemLimit(_component_conf &config, riaps::Actor &actor) :
      MemLimitBase(config, actor) {
      }
      
      void MemLimit::OnTicker(riaps::ports::PortBase *port) {
         std::cout << "MemLimit::OnTicker(): " << port->GetPortName() << std::endl;
      }
      
      void MemLimit::OnGroupMessage(const riaps::groups::GroupId& groupId,
      capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){
         
      }
      
      MemLimit::~MemLimit() {
         
      }
   }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
   auto result = new limits::components::MemLimit(config, actor);
   return result;
}

void destroy_component(riaps::ComponentBase *comp) {
   delete comp;
}

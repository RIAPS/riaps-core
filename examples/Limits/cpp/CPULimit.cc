#include <CPULimit.h>

namespace limits {
   namespace components {
      
      CPULimit::CPULimit(_component_conf &config, riaps::Actor &actor) :
      CPULimitBase(config, actor) {
      }
      
      void CPULimit::OnTicker(riaps::ports::PortBase *port) {
         std::cout << "CPULimit::OnTicker(): " << port->GetPortName() << std::endl;
      }
      
      void CPULimit::OnGroupMessage(const riaps::groups::GroupId& groupId,
      capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){
         
      }
      
      CPULimit::~CPULimit() {
         
      }
   }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
   auto result = new limits::components::CPULimit(config, actor);
   return result;
}

void destroy_component(riaps::ComponentBase *comp) {
   delete comp;
}

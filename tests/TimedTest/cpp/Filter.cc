#include <Filter.h>

namespace timedtest {
   namespace components {
      
      Filter::Filter(_component_conf &config, riaps::Actor &actor) :
      FilterBase(config, actor) {
      }
      
      void Filter::OnReady(const DummyT::Reader &message,
      riaps::ports::PortBase *port)
      {
         std::cout << "Filter::OnReady(): " << std::endl;
      }
      
      
      
      void Filter::OnGroupMessage(const riaps::groups::GroupId& groupId,
      capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){
         
      }
      
      Filter::~Filter() {
         
      }
   }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
   auto result = new timedtest::components::Filter(config, actor);
   return result;
}

void destroy_component(riaps::ComponentBase *comp) {
   delete comp;
}

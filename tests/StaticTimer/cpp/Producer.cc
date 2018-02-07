#include "include/Producer.h"

namespace statictimer {
   namespace components {
      
      Producer::Producer(_component_conf &config, riaps::Actor &actor) :
      ProducerBase(config, actor) {
      }
      
      void Producer::OnClock(riaps::ports::PortBase *port) {
        _logger->info("Send message");
          capnp::MallocMessageBuilder builder;
          auto msg = builder.initRoot<Msg>();
          msg.setValue("Ping");

          SendPing(builder, msg);
      }
      
      void Producer::OnGroupMessage(const riaps::groups::GroupId& groupId,
      capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){
         
      }
      
      Producer::~Producer() {
         
      }
   }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
   auto result = new statictimer::components::Producer(config, actor);
   return result;
}

void destroy_component(riaps::ComponentBase *comp) {
   delete comp;
}

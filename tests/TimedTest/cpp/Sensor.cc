#include <Sensor.h>

namespace timedtest {
   namespace components {
      
      Sensor::Sensor(_component_conf &config, riaps::Actor &actor) :
      SensorBase(config, actor) {
      }
      
      void Sensor::OnClock(riaps::ports::PortBase *port) {
         std::cout << "Sensor::OnClock(): " << port->GetPortName() << std::endl;
      }
      
      
      void Sensor::OnRequest(const DummyT::Reader &message,
      riaps::ports::PortBase *port)
      {
         std::cout<< "Sensor::OnRequest()"<< std::endl;
      }
      
      void Sensor::OnGroupMessage(const riaps::groups::GroupId& groupId,
      capnp::FlatArrayMessageReader& capnpreader, riaps::ports::PortBase* port){
         
      }
      
      Sensor::~Sensor() {
         
      }
   }
}

riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
   auto result = new timedtest::components::Sensor(config, actor);
   return result;
}

void destroy_component(riaps::ComponentBase *comp) {
   delete comp;
}

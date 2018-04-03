#include <Sensor.h>

namespace timedtest {
   namespace components {
      
      Sensor::Sensor(_component_conf &config, riaps::Actor &actor) :
      SensorBase(config, actor) {
      }
      
      void Sensor::OnClock(riaps::ports::PortBase *port) {
          capnp::MallocMessageBuilder builder;
          auto msg = builder.initRoot<DummyT>();
          msg.setMsg("0000");
          SendReady(builder, msg);
      }
      
      
      void Sensor::OnRequest(const DummyT::Reader &message,
      riaps::ports::PortBase *port)
      {
          if (port->GetPortBaseConfig()->isTimed) {
              _logger->info("{}, sendTs: {}.{}, recvTs: {}.{}", __FUNCTION__,
                            port->AsRecvPort()->GetLastSendTimestamp().tv_sec,
                            port->AsRecvPort()->GetLastSendTimestamp().tv_nsec,
                            port->AsRecvPort()->GetLastRecvTimestamp().tv_sec,
                            port->AsRecvPort()->GetLastRecvTimestamp().tv_nsec);
          } else {
              _logger->info("{}", __FUNCTION__);
          }

          capnp::MallocMessageBuilder builder;
          auto msg = builder.initRoot<DummyT>();
          msg.setMsg("FromSensor");
          SendRequest(builder, msg);
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

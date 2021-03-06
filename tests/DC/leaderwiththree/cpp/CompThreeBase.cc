//
// Auto-generated by edu.vanderbilt.riaps.generator.ComponenetGenerator.xtend
//
#include <CompThreeBase.h>

namespace leaderwithtree {
   namespace components {
      
      CompThreeBase::CompThreeBase(_component_conf &config, riaps::Actor &actor) : ComponentBase(config, actor) {
         
      }
      
      void CompThreeBase::DispatchMessage(capnp::FlatArrayMessageReader* capnpreader, riaps::ports::PortBase *port,std::shared_ptr<riaps::MessageParams> params) {
         auto portName = port->GetPortName();
         if (portName == PORT_TIMER_CLOCK) {
            OnClock(port);
         }
         
      }
      
      void CompThreeBase::DispatchInsideMessage(zmsg_t* zmsg, riaps::ports::PortBase* port) {
         //empty the header
      }
      
      
      CompThreeBase::~CompThreeBase() {
         
      }
   }
}

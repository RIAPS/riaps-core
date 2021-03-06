//
// Auto-generated by edu.vanderbilt.riaps.generator.ComponenetGenerator.xtend
//
#include "include/ProducerBase.h"

namespace statictimer {
   namespace components {
      
      ProducerBase::ProducerBase(_component_conf &config, riaps::Actor &actor) : ComponentBase(config, actor) {
         
      }
      
      void ProducerBase::DispatchMessage(capnp::FlatArrayMessageReader* capnpreader, riaps::ports::PortBase *port,std::shared_ptr<riaps::MessageParams> params) {
         auto portName = port->GetPortName();
         if (portName == PORT_TIMER_CLOCK) {
            OnClock(port);
         }
         
      }

       bool ProducerBase::SendPing(capnp::MallocMessageBuilder &messageBuilder,
                                   Msg::Builder &message) {
           std::cout<< "ProducerBase::SendPing()"<< std::endl;
           return SendMessageOnPort(messageBuilder, PORT_PUB_PING);
       }
      
      void ProducerBase::DispatchInsideMessage(zmsg_t* zmsg, riaps::ports::PortBase* port) {
         //empty the header
      }
      
      
      ProducerBase::~ProducerBase() {
         
      }
   }
}

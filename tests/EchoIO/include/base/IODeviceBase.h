
#ifndef RIAPS_CORE_IODEVICEBASE_H
#define RIAPS_CORE_IODEVICEBASE_H

#include "messages/EchoIO.capnp.h"
#include <componentmodel/r_componentbase.h>
//#include <messaging/insideport.capnp.h>

#define PORT_TIMER_CLOCK "clock"
#define PORT_REQ_ECHO "echo"
#define INSIDE_TRIGGER "trigger"

namespace echoio {
    namespace components {

        class IODeviceBase : public riaps::ComponentBase {
        public:
            IODeviceBase(_component_conf_j &config, riaps::Actor &actor);

            virtual void OnClock(riaps::ports::PortBase *port)=0;

            virtual void OnEcho(const messages::EchoRep::Reader &message,
                                riaps::ports::PortBase *port)=0;

            virtual void OnTrigger(zmsg_t* zmsg,
                                   riaps::ports::PortBase *port) = 0;

            virtual bool SendEcho(capnp::MallocMessageBuilder&    messageBuilder,
                                  messages::EchoReq::Builder& message);

            virtual bool SendTrigger(zmsg_t** message);

            virtual ~IODeviceBase();

        protected:

            virtual void DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                         riaps::ports::PortBase *port);

            virtual void DispatchInsideMessage(zmsg_t* zmsg,
                                               riaps::ports::PortBase* port);
        };
    }
}

#endif //RIAPS_CORE_IODEVICEBASE_H

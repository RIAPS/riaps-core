

#ifndef RIAPS_CORE_ECHOBASE_H
#define RIAPS_CORE_ECHOBASE_H

#define PORT_REP_ECHO "echo" // Timer, used only once for initialization
#define PORT_TIMER_CLOCK "clock"  // Request port to ask the echo server

#include <componentmodel/r_actor.h>
#include "messages/EchoIO.capnp.h"

namespace echoio {
    namespace components {

        class EchoBase : public riaps::ComponentBase {

        public:

            EchoBase(_component_conf_j &config, riaps::Actor &actor);

            virtual void OnClock(riaps::ports::PortBase *port)=0;

            virtual void OnEcho(const messages::EchoReq::Reader &message,
                                riaps::ports::PortBase *port)=0;

            virtual bool SendEcho(capnp::MallocMessageBuilder&    messageBuilder,
                                  messages::EchoRep::Builder& message);

            virtual ~EchoBase();

        protected:

            virtual void DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                         riaps::ports::PortBase *port);


        };
    }
}

#endif //RIAPS_CORE_ECHOBASE_H

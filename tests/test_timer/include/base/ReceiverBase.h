

#ifndef RIAPS_CORE_RECEIVERBASE_H
#define RIAPS_CORE_RECEIVERBASE_H

#include "componentmodel/r_componentbase.h"
#include <messages/timertest.capnp.h>

#define PORT_SUB_SIGNALVALUE "signalValue"

namespace timertest {
    namespace components {

        class ReceiverBase : public riaps::ComponentBase {

        public:

            ReceiverBase(_component_conf_j &config, riaps::Actor &actor);

            virtual void OnSignalValue(const messages::SignalValue::Reader &message,
                                       riaps::ports::PortBase *port)=0;


            virtual ~ReceiverBase();

        protected:

            virtual void DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                         riaps::ports::PortBase *port);


        };
    }
}

#endif

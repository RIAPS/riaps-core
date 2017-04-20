

#ifndef RIAPS_CORE_GENERATORBASE_H
#define RIAPS_CORE_GENERATORBASE_H

#include "componentmodel/r_componentbase.h"
#include <messages/timertest.capnp.h>

// Name of the ports from the model file
#define PORT_TIMER_CLOCK "clock"
#define PORT_PUB_SIGNALVALUE "signalValue"

namespace timertest {
    namespace components {

        class GeneratorBase : public riaps::ComponentBase {

        public:

            GeneratorBase(_component_conf_j &config, riaps::Actor &actor);
            virtual void OnClock(riaps::ports::PortBase *port)=0;
            virtual bool SendSignalValue(capnp::MallocMessageBuilder&    messageBuilder,
                                        messages::SignalValue::Builder& message);
            virtual ~GeneratorBase();

        protected:

            virtual void DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                         riaps::ports::PortBase *port);


        };
    }
}

#endif

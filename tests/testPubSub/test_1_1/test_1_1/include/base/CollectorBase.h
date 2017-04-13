//
// Created by istvan on 4/13/17.
//

#ifndef RIAPS_CORE_COLLECTORBASE_H
#define RIAPS_CORE_COLLECTORBASE_H


#include "componentmodel/r_componentbase.h"
#include "messages/testmessages.capnp.h"

#define PORT_SUB_GETTEMPERATURE "getTemperature"

namespace testing {
    namespace components{
        class CollectorBase : public riaps::ComponentBase {

        public:

            CollectorBase(_component_conf_j& config, riaps::Actor& actor);

            virtual void OnGetTemperature(testing::messages::SensorValue::Reader& message,
                                          riaps::ports::PortBase* port)=0;

            virtual ~CollectorBase();

        protected:
            virtual void DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                         riaps::ports::PortBase*   port);

        };
    }
}


#endif //RIAPS_CORE_COLLECTORBASE_H

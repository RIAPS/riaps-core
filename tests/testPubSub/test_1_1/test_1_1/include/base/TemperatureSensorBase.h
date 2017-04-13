//
// Created by istvan on 4/13/17.
//

#ifndef RIAPS_CORE_TEMPERATURESENSORBASE_H
#define RIAPS_CORE_TEMPERATURESENSORBASE_H

#include "componentmodel/r_componentbase.h"
#include "messages/testmessages.capnp.h"

#define PORT_TIMER_CLOCK "clock"
#define PORT_PUB_TEMPERATURE "temperature"

namespace testing{
    namespace components{

        class TemperatureSensorBase : public riaps::ComponentBase {

        public:

            TemperatureSensorBase(_component_conf_j& config, riaps::Actor& actor);

            virtual void OnClock(riaps::ports::PortBase *port)=0;

            virtual bool SendTemperature(capnp::MallocMessageBuilder&    messageBuilder,
                                         messages::SensorValue::Builder& message);


            virtual ~TemperatureSensorBase();

        protected:
            virtual void DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                         riaps::ports::PortBase*   port);

            std::string  _logfilePath;
            std::fstream _logStream;
            int          _messageCounter;

        };

    }
}

#endif //RIAPS_CORE_TEMPERATURESENSORBASE_H

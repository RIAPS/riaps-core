//
// Created by istvan on 3/10/17.
//

#ifndef RIAPS_CORE_SENSORBASE_H
#define RIAPS_CORE_SENSORBASE_H

#include "componentmodel/r_componentbase.h"
#include "messages/distributedestimator.capnp.h"

// Name of the ports from the model file
#define PORT_TIMER_CLOCK "clock"
#define PORT_PUB_READY   "ready"
#define PORT_REP_REQUEST "request"

namespace distributedestimator {
    namespace components {

        class comp_sensorbase : public riaps::ComponentBase {

        public:

            comp_sensorbase(_component_conf_j &config, riaps::Actor &actor);

            //virtual void RegisterHandlers();

            virtual void OnClock(riaps::ports::PortBase *port)=0;

            // No handler for publisher
            // But send function for publisher, request, response makes sense, maybe easier for the developer
            //virtual void OnReady(const std::string& messagetype,
            //                     std::vector<std::string>& msgFields,
            //                     riaps::ports::PortBase* port)=0;

            virtual void OnRequest(const messages::SensorQuery::Reader &message,
                                   riaps::ports::PortBase *port)=0;

            virtual bool SendRequest(capnp::MallocMessageBuilder&    messageBuilder,
                                     messages::SensorValue::Builder& message);

            virtual bool SendReady(capnp::MallocMessageBuilder&    messageBuilder,
                                   messages::SensorReady::Builder& message);


            virtual ~comp_sensorbase();

        protected:

            virtual void DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                         riaps::ports::PortBase *port);


        };
    }
}

#endif //RIAPS_CORE_SENSORBASE_H

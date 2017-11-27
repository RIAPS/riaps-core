//
// Created by istvan on 3/10/17.
//

#ifndef RIAPS_CORE_SENSORBASE_H
#define RIAPS_CORE_SENSORBASE_H

#include "componentmodel/r_componentbase.h"
#include "messages/activereplica.capnp.h"
#include "GroupTypes.h"

// Name of the ports from the model file
#define PORT_TIMER_CLOCK "clock"
#define PORT_PUB_READY   "ready"
#define PORT_REP_REQUEST "request"

namespace activereplica {
    namespace components {

        class SensorBase : public riaps::ComponentBase {

        public:
            SensorBase(_component_conf &config, riaps::Actor &actor);
            virtual ~SensorBase();

        protected:

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




        private:

            virtual void DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                         riaps::ports::PortBase *port,
                                         std::shared_ptr<riaps::AsyncInfo> asyncInfo = nullptr);

            virtual void DispatchInsideMessage(zmsg_t* zmsg,
                                               riaps::ports::PortBase* port);


        };
    }
}

#endif //RIAPS_CORE_SENSORBASE_H

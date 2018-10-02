//
// Created by istvan on 3/10/17.
//

#ifndef RIAPS_CORE_LOCALESTIMATORBASE_H
#define RIAPS_CORE_LOCALESTIMATORBASE_H

#include <componentmodel/r_riapsmessage.h>
#include "componentmodel/r_componentbase.h"
#include "messages/activereplica.capnp.h"
#include "GroupTypes.h"

// Name of the ports from the model file
#define PORT_SUB_READY    "ready"
#define PORT_QRY_QUERY    "query"
#define PORT_PUB_ESTIMATE "estimate"



namespace activereplica {
    namespace components {

        class ServerBase : public riaps::ComponentBase {

        public:

            ServerBase(_component_conf &config, riaps::Actor &actor);

            virtual void OnReady(const messages::SensorReady::Reader &message,
                                 riaps::ports::PortBase *port)=0;

            bool SendQuery(capnp::MallocMessageBuilder&    messageBuilder,
                           messages::SensorQuery::Builder& message,
                           std::string& requestId);

            bool RecvQuery(std::shared_ptr<riaps::RiapsMessage<messages::SensorValue::Reader, messages::SensorValue>>& message,
                           std::shared_ptr<riaps::MessageParams>& params);

            virtual void OnQuery(std::shared_ptr<riaps::RiapsMessage<messages::SensorValue::Reader, messages::SensorValue>>& message,
                                 riaps::ports::PortBase *port,
                                 std::shared_ptr<riaps::MessageParams> params)=0;

            bool SendEstimate(capnp::MallocMessageBuilder& messageBuilder,
                              messages::Estimate::Builder& message);


            virtual ~ServerBase();

        protected:
            virtual void DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                         riaps::ports::PortBase *port,
                                         std::shared_ptr<riaps::MessageParams> payload = nullptr);

            virtual void DispatchInsideMessage(zmsg_t* zmsg,
                                               riaps::ports::PortBase* port);

        };
    }
}

#endif //RIAPS_CORE_LOCALESTIMATORBASE_H

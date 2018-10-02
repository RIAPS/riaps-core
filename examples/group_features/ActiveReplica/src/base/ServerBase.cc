//
// Created by istvan on 3/10/17.
//

#include <base/ServerBase.h>

namespace activereplica {
    namespace components {

        ServerBase::ServerBase(_component_conf &config, riaps::Actor &actor) : ComponentBase(config,
                                                                                                             actor) {

        }

        void ServerBase::DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                         riaps::ports::PortBase *port,
                                         std::shared_ptr<riaps::MessageParams> payload) {
            if (port->GetPortName() == PORT_SUB_READY) {
                messages::SensorReady::Reader sensorReady = capnpreader->getRoot<messages::SensorReady>();
                OnReady(sensorReady, port);
            } else if (port->GetPortName() == PORT_QRY_QUERY){
                auto messagePtr =
                             std::shared_ptr<riaps::RiapsMessage<messages::SensorValue::Reader, messages::SensorValue>>(
                               new riaps::RiapsMessage<messages::SensorValue::Reader, messages::SensorValue>(&capnpreader)
                             );
                OnQuery(messagePtr, port, payload);
            }
        }

        bool ServerBase::SendQuery(capnp::MallocMessageBuilder &messageBuilder,
                                           messages::SensorQuery::Builder &message,
                                          std::string& requestId) {
            return SendMessageOnPort(messageBuilder, PORT_QRY_QUERY, requestId);
        }

        bool ServerBase::RecvQuery(std::shared_ptr<riaps::RiapsMessage<messages::SensorValue::Reader, messages::SensorValue>>& message,
                                   std::shared_ptr<riaps::MessageParams>& params) {
            auto port = GetQueryPortByName(PORT_QRY_QUERY);
            if (port == NULL) return false;

            return port->RecvQuery<messages::SensorValue::Reader, messages::SensorValue>(message, params);

        }

        void ServerBase::DispatchInsideMessage(zmsg_t *zmsg, riaps::ports::PortBase *port) {

        }

        bool ServerBase::SendEstimate(capnp::MallocMessageBuilder &messageBuilder,
                                              messages::Estimate::Builder &message) {
            return SendMessageOnPort(messageBuilder, PORT_PUB_ESTIMATE);
        }

        ServerBase::~ServerBase() {

        }
    }
}
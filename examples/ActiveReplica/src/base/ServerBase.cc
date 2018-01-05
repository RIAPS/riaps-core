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
                                                 riaps::ports::PortBase *port) {
            if (port->GetPortName() == PORT_SUB_READY) {
                messages::SensorReady::Reader sensorReady = capnpreader->getRoot<messages::SensorReady>();
                OnReady(sensorReady, port);
            }
        }

        bool ServerBase::SendQuery(capnp::MallocMessageBuilder &messageBuilder,
                                           messages::SensorQuery::Builder &message) {
            return SendMessageOnPort(messageBuilder, PORT_REQ_QUERY);
        }

        bool ServerBase::RecvQuery(messages::SensorValue::Reader &message) {
            auto port = GetRequestPortByName(PORT_REQ_QUERY);
            if (port == NULL) return false;

            capnp::FlatArrayMessageReader* messageReader;

            if (port->Recv(&messageReader)){
                message = messageReader->getRoot<messages::SensorValue>();
                return true;
            }
            return false;
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
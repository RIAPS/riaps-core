//
// Created by istvan on 3/10/17.
//

#include <base/LocalEstimatorBase.h>

namespace distributedestimator {
    namespace components {

        LocalEstimatorBase::LocalEstimatorBase(_component_conf_j &config, riaps::Actor &actor) : ComponentBase(config,
                                                                                                               actor) {

        }

        void LocalEstimatorBase::DispatchMessage(const std::string &messagetype,
                                                 capnp::FlatArrayMessageReader* capnpreader,
                                                 riaps::ports::PortBase *port) {
            if (port->GetPortName() == PORT_SUB_READY) {
                messages::SensorReady::Reader sensorReady = capnpreader->getRoot<messages::SensorReady>();
                OnReady(messagetype, sensorReady, port);
            }
        }

        bool LocalEstimatorBase::SendQuery(capnp::MallocMessageBuilder &messageBuilder,
                                           messages::SensorQuery::Builder &message) {
            return SendMessageOnPort(messageBuilder, PORT_REQ_QUERY);
        }

        bool LocalEstimatorBase::RecvQuery(std::string &messageType, messages::SensorValue::Reader &message) {
            auto port = GetRequestPortByName(PORT_REQ_QUERY);
            if (port == NULL) return false;

            capnp::FlatArrayMessageReader* messageReader;

            if (port->Recv(messageType, &messageReader)){
                message = messageReader->getRoot<messages::SensorValue>();
                return true;
            }
            return false;
        }

        bool LocalEstimatorBase::SendEstimate(capnp::MallocMessageBuilder &messageBuilder,
                                              messages::Estimate::Builder &message) {
            return SendMessageOnPort(messageBuilder, PORT_PUB_ESTIMATE);
        }

        LocalEstimatorBase::~LocalEstimatorBase() {

        }
    }
}
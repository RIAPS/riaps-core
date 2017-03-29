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
                messages::SensorReady sensorReady;
                sensorReady.InitReader(capnpreader);
                OnReady(messagetype, sensorReady, port);
            }
        }

        bool LocalEstimatorBase::SendQuery(messages::SensorQuery &message) {
            return SendMessageOnPort(&message, PORT_REQ_QUERY);
        }

        bool LocalEstimatorBase::RecvQuery(std::string &messageType, messages::SensorValue &message) {
            auto port = GetRequestPortByName(PORT_REQ_QUERY);
            if (port == NULL) return false;

            return port->Recv(messageType, &message);
        }

        bool LocalEstimatorBase::SendEstimate(messages::Estimate &message) {
            return SendMessageOnPort(&message, PORT_PUB_ESTIMATE);
        }

        LocalEstimatorBase::~LocalEstimatorBase() {

        }
    }
}
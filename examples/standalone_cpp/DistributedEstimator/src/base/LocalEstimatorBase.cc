//
// Created by istvan on 3/10/17.
//

#include <base/LocalEstimatorBase.h>

namespace distributedestimator {
    namespace components {

        LocalEstimatorBase::LocalEstimatorBase(_component_conf &config, riaps::Actor &actor) : ComponentBase(config,
                                                                                                             actor) {

        }

        void LocalEstimatorBase::DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                                 riaps::ports::PortBase *port,
                                                 std::shared_ptr<riaps::MessageParams> params) {
            if (port->GetPortName() == PORT_SUB_READY) {
                messages::SensorReady::Reader sensorReady = capnpreader->getRoot<messages::SensorReady>();
                OnReady(sensorReady, port);
            }
        }

        bool LocalEstimatorBase::SendQuery(capnp::MallocMessageBuilder &messageBuilder,
                                           messages::SensorQuery::Builder &message) {
            return SendMessageOnPort(messageBuilder, PORT_REQ_QUERY);
        }

        bool LocalEstimatorBase::RecvQuery(messages::SensorValue::Reader &message) {
            auto port = GetRequestPortByName(PORT_REQ_QUERY);
            if (port == NULL) return false;

            // TODO: with share_ptr it would be better, DSML update is also required
            capnp::FlatArrayMessageReader* messageReader;

            if (port->Recv(&messageReader)){
                message = messageReader->getRoot<messages::SensorValue>();
                return true;
            }

            return false;
        }

        void LocalEstimatorBase::DispatchInsideMessage(zmsg_t *zmsg, riaps::ports::PortBase *port) {

        }

        bool LocalEstimatorBase::SendEstimate(capnp::MallocMessageBuilder &messageBuilder,
                                              messages::Estimate::Builder &message) {
            return SendMessageOnPort(messageBuilder, PORT_PUB_ESTIMATE);
        }

        LocalEstimatorBase::~LocalEstimatorBase() {

        }
    }
}
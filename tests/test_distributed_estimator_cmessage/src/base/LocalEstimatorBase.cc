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
                                                 kj::ArrayPtr<const capnp::word>* data,
                                                 riaps::ports::PortBase *port) {
            if (port->GetPortName() == PORT_SUB_READY) {

                messages::SensorReady sensorReady(*data);

                OnReady(messagetype, sensorReady, port);
            }

        }

        bool LocalEstimatorBase::SendQuery(const messages::SensorQuery &message) {
            //msgpack::sbuffer sbuf;
            //msgpack::pack(sbuf, message);

            //return SendMessageOnPort(NULL, PORT_REQ_QUERY);
            return true;
        }

        bool LocalEstimatorBase::RecvQuery(std::string &messageType, messages::SensorValue &message) {
            auto port = GetRequestPortByName(PORT_REQ_QUERY);
            if (port == NULL) return false;

//            if (port->Recv(messageType, sbuf)) {
//                msgpack::unpacked msg;
//                msgpack::unpack(&msg, sbuf.data(), sbuf.size());
//                msgpack::object obj = msg.get();
//
//                obj.convert(&message);
//                return true;
//            }
            return false;

        }

        bool LocalEstimatorBase::SendEstimate(const messages::Estimate &message) {
            //msgpack::sbuffer sbuf;
            //msgpack::pack(sbuf, message);
            return true;
            //return SendMessageOnPort(sbuf, PORT_PUB_ESTIMATE);
        }

        LocalEstimatorBase::~LocalEstimatorBase() {

        }
    }
}
//
// Created by istvan on 3/10/17.
//

#include <base/LocalEstimatorBase.h>

LocalEstimatorBase::LocalEstimatorBase(_component_conf_j &config, riaps::Actor &actor) : ComponentBase(config, actor){

}

void LocalEstimatorBase::DispatchMessage(const std::string &messagetype,
                                         msgpack::sbuffer *message,
                                         riaps::ports::PortBase *port) {
    if (port->GetPortName() == PORT_SUB_READY){
        msgpack::unpacked msg;
        msgpack::unpack(&msg, message->data(), message->size());
        msgpack::object obj = msg.get();
        messages::SensorReady sensorReady;
        obj.convert(&sensorReady);

        OnReady(messagetype, sensorReady, port);
    }

}

bool LocalEstimatorBase::SendQuery(const messages::SensorQuery &message) {
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, message);

    return SendMessageOnPort(sbuf, PORT_REQ_QUERY);
}

bool LocalEstimatorBase::RecvQuery(std::string& messageType, messages::SensorValue &message) {
    auto port = GetRequestPortByName(PORT_REQ_QUERY);
    if (port == NULL) return false;

    msgpack::sbuffer sbuf;

    if (port->Recv(messageType, sbuf)){
        msgpack::unpacked msg;
        msgpack::unpack(&msg, sbuf.data(), sbuf.size());
        msgpack::object obj = msg.get();

        obj.convert(&message);
        return true;
    }
    return false;

}

LocalEstimatorBase::~LocalEstimatorBase() {

}
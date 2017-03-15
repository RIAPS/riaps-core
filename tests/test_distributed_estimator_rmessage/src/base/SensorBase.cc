//
// Created by istvan on 3/10/17.
//

#include <base/SensorBase.h>



comp_sensorbase::comp_sensorbase(_component_conf_j &config, riaps::Actor &actor):ComponentBase(config, actor) {

}

void comp_sensorbase::DispatchMessage(const std::string &messagetype, msgpack::sbuffer *message,
                                      riaps::ports::PortBase *port) {
    auto portName = port->GetPortName();
    if (portName == PORT_TIMER_CLOCK){
        OnClock(messagetype, port);
    } else if (portName == PORT_REP_REQUEST){
        msgpack::unpacked msg;
        msgpack::unpack(&msg, message->data(), message->size());
        msgpack::object obj = msg.get();
        messages::SensorQuery sensorQuery;
        obj.convert(&sensorQuery);
        OnRequest(messagetype, sensorQuery, port);
    }

}

//void comp_sensorbase::RegisterHandlers() {
//    RegisterHandler(PORT_REP_REQUEST, reinterpret_cast<riaps::riaps_handler>(&comp_sensorbase::OnRequest));
//    RegisterHandler(PORT_TIMER_CLOCK, reinterpret_cast<riaps::riaps_handler>(&comp_sensorbase::OnClock));
//}

bool comp_sensorbase::SendRequest(const messages::SensorValue &message) {
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, message);

    return SendMessageOnPort(sbuf, PORT_REP_REQUEST);
}

bool comp_sensorbase::SendReady(const messages::SensorReady &message) {
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, message);

    return SendMessageOnPort(sbuf, PORT_PUB_READY);
}

comp_sensorbase::~comp_sensorbase() {

}
//
// Created by istvan on 5/16/17.
//

#include <base/GpioDeviceComponentBase.h>

namespace gpiotoggleexample{
    namespace components{
        GpioDeviceComponentBase::GpioDeviceComponentBase(_component_conf &config, riaps::Actor &actor)
            : ComponentBase(config, actor){

        }

        void GpioDeviceComponentBase::DispatchMessage(capnp::FlatArrayMessageReader *capnpreader,
                                                      riaps::ports::PortBase *port,
                                                      std::shared_ptr<riaps::MessageParams> params) {
            auto portName = port->GetPortName();
            if (portName == TIMER_CLOCK) {
                OnClock(port);
            } else if (portName == PORT_SUB_READGPIO) {
                auto readRequest = capnpreader->getRoot<messages::ReadRequest>();
                OnReadGpio(readRequest, port);
            } else if (portName == PORT_SUB_WRITEGPIO){
                auto writeRequest = capnpreader->getRoot<messages::WriteRequest>();
                OnWriteGpio(writeRequest, port);
            }
        }

        void GpioDeviceComponentBase::DispatchInsideMessage(zmsg_t *zmsg, riaps::ports::PortBase *port) {
            auto portName = port->GetPortName();
            if (portName == INSIDE_DATAQUEUE) {
                OnDataQueue(zmsg, port);
            }
        }

        bool GpioDeviceComponentBase::SendReportedData(capnp::MallocMessageBuilder &messageBuilder,
                                                       messages::DataValue::Builder &message) {
            return SendMessageOnPort(messageBuilder, PORT_PUB_REPORTEDDATA);
        }

        bool GpioDeviceComponentBase::SendDataQueue(zmsg_t **message) {
            return SendMessageOnPort(message, INSIDE_DATAQUEUE);
        }

        GpioDeviceComponentBase::~GpioDeviceComponentBase() {

        }
    }
}
//
// Created by istvan on 5/16/17.
//

#include <base/GpioDeviceComponentBase.h>

namespace gpiotoggleexample{
    namespace components{
        GpioDeviceComponentBase::GpioDeviceComponentBase(_component_conf_j &config, riaps::Actor &actor)
            : ComponentBase(config, actor){

        }

        void GpioDeviceComponentBase::DispatchMessage(capnp::FlatArrayMessageReader *capnpreader,
                                                      riaps::ports::PortBase *port) {
            auto portName = port->GetPortName();
            if (portName == TIMER_CLOCK) {
                OnClock(port);
            } else if (portName == PORT_SUB_READGPIO) {
                auto readRequest = capnpreader->getRoot<messages::ReadRequest>();
                OnReadGpio(readRequest, port);
            } else if (portName == PORT_SUB_WRITEGPIO){
                auto writeRequest = capnpreader->getRoot<messages::WriteRequest>();
                OnWriteGpio(writeRequest, port);
            } else if (portName == INSIDE_DATAIN_QUEUE){
                auto insideMessage = capnpreader->getRoot<riaps::ports::InsideMessage>();
                OnDataInQueue(insideMessage, port);
            } else if (portName == INSIDE_DATAOUT_QUEUE){
                auto insideMessage = capnpreader->getRoot<riaps::ports::InsideMessage>();
                OnDataInQueue(insideMessage, port);
            }
        }

        bool GpioDeviceComponentBase::SendReportedData(capnp::MallocMessageBuilder &messageBuilder,
                                                       messages::DataValue::Builder &message) {
            return SendMessageOnPort(messageBuilder, PORT_PUB_REPORTEDDATA);
        }

        bool GpioDeviceComponentBase::SendDataOutQueue(capnp::MallocMessageBuilder&    messageBuilder,
                                                       riaps::ports::InsideMessage::Builder& message){
            return SendMessageOnPort(messageBuilder, INSIDE_DATAOUT_QUEUE);
        }

        bool GpioDeviceComponentBase::SendDataInQueue(capnp::MallocMessageBuilder&    messageBuilder,
                                                      riaps::ports::InsideMessage::Builder& message){
            return SendMessageOnPort(messageBuilder, INSIDE_DATAIN_QUEUE);
        }

        GpioDeviceComponentBase::~GpioDeviceComponentBase() {

        }
    }
}
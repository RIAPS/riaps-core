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
                std::cout << "Inside ports are not implemented" << std::endl;
            } else if (portName == INSIDE_DATAOUT_QUEUE){
                std::cout << "Inside ports are not implemented" << std::endl;
            }
        }

        bool GpioDeviceComponentBase::SendReportedData(capnp::MallocMessageBuilder &messageBuilder,
                                                       messages::DataValue::Builder &message) {
            return SendMessageOnPort(messageBuilder, PORT_PUB_REPORTEDDATA);
        }

        GpioDeviceComponentBase::~GpioDeviceComponentBase() {

        }
    }
}
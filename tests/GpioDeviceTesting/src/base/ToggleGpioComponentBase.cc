//
// Created by istvan on 5/16/17.
//

#include <base/ToggleGpioComponentBase.h>

namespace gpiotoggleexample {
    namespace components {

        ToggleGpioComponentBase::ToggleGpioComponentBase(_component_conf_j &config, riaps::Actor &actor)
            : ComponentBase(config, actor){

        }

        bool ToggleGpioComponentBase::SendPollGpioValue(capnp::MallocMessageBuilder &messageBuilder,
                                                        messages::ReadRequest::Builder &message) {
            return SendMessageOnPort(messageBuilder, PORT_PUB_POLLGPIOVAPUE);
        }

        bool ToggleGpioComponentBase::SendWriteGpioValue(capnp::MallocMessageBuilder &messageBuilder,
                                                         messages::WriteRequest::Builder &message) {
            return SendMessageOnPort(messageBuilder, PORT_PUB_WRITEGPIOVALUE);
        }

        void ToggleGpioComponentBase::DispatchMessage(capnp::FlatArrayMessageReader *capnpreader,
                                                      riaps::ports::PortBase *port) {
            auto portName = port->GetPortName();
            if (portName == PORT_SUB_CURRENTGPIOVALUE) {
                auto dataValue = capnpreader->getRoot<messages::DataValue>();
                OnCurrentGpioValue(dataValue, port);
            } else if (portName == TIMER_TOGGLE) {
                OnToggle(port);
            } else if (portName == TIMER_READVALUE){
                OnReadValue(port);
            }
        }

        ToggleGpioComponentBase::~ToggleGpioComponentBase() {

        }

    }
}
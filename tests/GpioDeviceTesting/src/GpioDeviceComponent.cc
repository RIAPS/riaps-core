//
// Created by istvan on 5/16/17.
//
#include <GpioDeviceComponent.h>

namespace gpiotoggleexample{
    namespace components{
        GpioDeviceComponent::GpioDeviceComponent(_component_conf_j &config, riaps::Actor &actor)
            : GpioDeviceComponentBase(config, actor),
              _deviceThread(nullptr){

        }

        void GpioDeviceComponent::OnReadGpio(const messages::ReadRequest::Reader &message,
                                             riaps::ports::PortBase *port) {
            if (_deviceThread!= nullptr && _deviceThread->IsGpioAvailable()){
                std::cout << "on_readGpio(): " << message.getMsg().cStr() << std::endl;
                _deviceThread->RequestGpioRead();
            }
            else{
                std::cout << "on_readGpio()[" << getpid() << "]: GPIO not available yet" << std::endl;
            }
        }

        void GpioDeviceComponent::OnWriteGpio(const messages::WriteRequest::Reader &message,
                                              riaps::ports::PortBase *port) {
            if (_deviceThread!= nullptr && _deviceThread->IsGpioAvailable()){
                std::cout << "on_writeGpio() write " << message.getValue() << std::endl;

                capnp::MallocMessageBuilder builder;
                auto insideMessage = builder.initRoot<riaps::ports::InsideMessage>();
                insideMessage.setValue(std::to_string(message.getValue()));
                SendDataOutQueue(builder, insideMessage);
                _deviceThread->RequestGpioWrite();
            }
            else{
                std::cout << "on_writeGpio()[" << getpid() << "]: GPIO not available yet" << std::endl;
            }
        }

        void GpioDeviceComponent::OnDataInQueue(const riaps::ports::InsideMessage::Reader &message,
                                                riaps::ports::PortBase *port) {
            std::cout << "GpioDeviceComponent::OnDataInQueue()[" << getpid() << "]: " << message.getValue().cStr() << std::endl;

            capnp::MallocMessageBuilder builder;
            auto dataValue = builder.initRoot<gpiotoggleexample::messages::DataValue>();

            dataValue.setValue(atoi(message.getValue().cStr()));
            SendReportedData(builder, dataValue);
            std::cout << "GpioDeviceComponent::OnDataInQueue(): published GPIO value = " << message.getValue().cStr() << std::endl;
        }

        void GpioDeviceComponent::OnDataOutQueue(const riaps::ports::InsideMessage::Reader &message,
                                                riaps::ports::PortBase *port) {

        }

        void GpioDeviceComponent::OnClock(riaps::ports::PortBase *port) {
            std::cout << "GpioDeviceComponent::OnClock()[" << getpid() <<"]" <<std::endl;

            if (_deviceThread == nullptr){
                _deviceThread = std::unique_ptr<GpioDeviceThread>(new GpioDeviceThread(GetConfig()));
                _deviceThread->StartThread();
            }
        }

        void GpioDeviceComponent::OnOneShotTimer(const std::string &) {

        }

        GpioDeviceComponent::~GpioDeviceComponent() {
            _deviceThread->Terminate();
        }
    }
}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    auto result = new gpiotoggleexample::components::GpioDeviceComponent(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
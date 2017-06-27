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
                zmsg_t* msg = zmsg_new();
                zmsg_addstr(msg, CMD_READ_REQUEST);
                SendDataQueue(&msg);
            }
            else{
                std::cout << "on_readGpio()[" << getpid() << "]: GPIO not available yet" << std::endl;
            }
        }



        void GpioDeviceComponent::OnWriteGpio(const messages::WriteRequest::Reader &message,
                                              riaps::ports::PortBase *port) {
            if (_deviceThread!= nullptr && _deviceThread->IsGpioAvailable()){
                std::cout << "on_writeGpio() write " << message.getValue().cStr() << std::endl;
                zmsg_t* msg = zmsg_new();
                zmsg_addstr(msg, CMD_WRITE_REQUEST);
                SendDataQueue(&msg);
            }
            else{
                std::cout << "on_writeGpio()[" << getpid() << "]: GPIO not available yet" << std::endl;
            }
        }

        void GpioDeviceComponent::OnDataQueue(zmsg_t *zmsg, riaps::ports::PortBase *port) {
            char* value = zmsg_popstr(zmsg);
            std::cout << "GpioDeviceComponent::OnDataInQueue()[" << getpid() << "]: " << value << std::endl;

            capnp::MallocMessageBuilder builder;
            auto dataValue = builder.initRoot<gpiotoggleexample::messages::DataValue>();
            dataValue.setValue(value);
            SendReportedData(builder, dataValue);

            std::cout << "GpioDeviceComponent::OnDataInQueue(): published GPIO value = " << value << std::endl;
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
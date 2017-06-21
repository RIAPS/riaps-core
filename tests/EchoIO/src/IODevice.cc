//
// Created by istvan on 6/20/17.
//

#include <IODevice.h>

namespace echoio{
    namespace components {

        IODevice::IODevice(_component_conf_j &config, riaps::Actor &actor)
            : IODeviceBase(config, actor),
              _deviceThread(nullptr){
        }

        void IODevice::OnClock(riaps::ports::PortBase *port){
            std::cout << "IODevice::OnClock()" << std::endl;
            if (_deviceThread == nullptr){
                _deviceThread = std::unique_ptr<IODeviceThread>(new IODeviceThread(GetConfig()));
                _deviceThread->StartThread();
            }
        }

        void IODevice::OnEcho(const messages::EchoRep::Reader &message, riaps::ports::PortBase *port) {

            zmsg_t* zmsg = zmsg_new();
            zmsg_addstr(zmsg, message.getMsg().cStr());

            // Send message to the inner thread, using inside ports.
            SendTrigger(&zmsg);

        }

        void IODevice::OnTrigger(zmsg_t* zmsg, riaps::ports::PortBase *port) {

            char* message = zmsg_popstr(zmsg);

            // Send it to the echo server
            capnp::MallocMessageBuilder builder;
            auto echoReq = builder.initRoot<echoio::messages::EchoReq>();
            echoReq.setValue(message);

            SendEcho(builder, echoReq);
        }

        void IODevice::OnOneShotTimer(const std::string &timerid) {

        }

        IODevice::~IODevice() {}
    }
}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    auto result = new echoio::components::IODevice(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
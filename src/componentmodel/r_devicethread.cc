//
// Created by istvan on 5/17/17.
//


#include <componentmodel/r_devicethread.h>
#include <iostream>

namespace riaps{
    namespace components{


        DeviceThread::DeviceThread(const _component_conf_j& deviceConfig)
                : _deviceConfig(deviceConfig){
            _isTerminated.store(false);
        }

        // Starts the zactor
        // Inside ports are initialized by the actor
        void DeviceThread::StartThread() {
            _deviceThread = std::thread(&DeviceThread::Run, this);
        }

        bool DeviceThread::IsTerminated() const {
            return _isTerminated.load();
        }

        void DeviceThread::Terminate() {
            std::cout << "Terminate called" <<std::endl;
            _isTerminated.store(true);
            if (_deviceThread.joinable())
                _deviceThread.join();
        }

        void DeviceThread::InitInsides(zpoller_t *poller) {

            // Add insider ports
            for (auto&& it_insconf = _deviceConfig.component_ports.inss.begin();
                 it_insconf != _deviceConfig.component_ports.inss.end();
                 it_insconf++){

                auto newPortPtr = new ports::InsidePort(*it_insconf, riaps::ports::InsidePortMode::CONNECT, NULL);
                std::unique_ptr<ports::PortBase> newport(newPortPtr);
                _insidePorts[it_insconf->portName] = std::move(newport);

                if (poller!=NULL) {
                    zpoller_add(poller, (void *) newPortPtr->GetSocket());
                }
            }
        }

        bool DeviceThread::SendMessageOnPort(capnp::MallocMessageBuilder& message, const std::string &portName) {
            auto serializedMessage = capnp::messageToFlatArray(message);
            zmsg_t* msg = zmsg_new();
            auto bytes = serializedMessage.asBytes();
            zmsg_pushmem(msg, bytes.begin(), bytes.size());
            return SendMessageOnPort(&msg, portName);
        }

        bool DeviceThread::SendMessageOnPort(zmsg_t **message, const std::string &portName) {
            auto port = GetInsidePortByName(portName);
            if (port == NULL) return false;
            return port->Send(message);
        }



        riaps::ports::InsidePort* DeviceThread::GetInsidePortByName(const std::string &portName) {
            if (_insidePorts.find(portName)!=_insidePorts.end()){
                return ((riaps::ports::InsidePort*)_insidePorts[portName].get());
            }
            return NULL;
        }

        DeviceThread::~DeviceThread() {
            for(auto it = _insidePorts.begin(); it!=_insidePorts.end(); it++){
                auto ptr = it->second.release();
                delete ptr;
            }
           Terminate();
        }

//        void devThreadActor(zsock_t* pipe, void* args){
//            DeviceThread* devThreadObj = (DeviceThread*) args;
//
//
//            zpoller_t* poller = zpoller_new(pipe, NULL);
//            assert(poller);
//            zpoller_ignore_interrupts (poller);
//
//            devThreadObj->InitInsides(poller);
//
//            zsock_signal (pipe, 0);
//            bool terminated = false;
//            while (!terminated) {
//                void *which = zpoller_wait(poller, 500);
//                if (which == pipe) {
//                    zmsg_t *msg = zmsg_recv(which);
//                    if (!msg) {
//                        std::cout << "No msg => interrupted" << std::endl;
//                        break;
//                    }
//
//                    char *command = zmsg_popstr(msg);
//
//                    if (streq(command, "$TERM")) {
//                        std::cout << "$TERM arrived in component" << std::endl;
//                        terminated = true;
//                    }
//                }
//            }
//        }
    }
}
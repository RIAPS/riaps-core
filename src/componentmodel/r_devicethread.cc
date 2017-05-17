//
// Created by istvan on 5/17/17.
//


#include <componentmodel/r_devicethread.h>
#include <iostream>

namespace riaps{
    namespace components{


        DeviceThread::DeviceThread(_component_conf_j& deviceConfig)
                : _deviceThread(NULL),
                  _deviceConfig(deviceConfig){

        }

        // Starts the zactor
        // Inside ports are initialized by the actor
        void DeviceThread::StartThread() {
            zactor_new(devThreadActor, this);
        }

        void DeviceThread::StopThread() {
            if (_deviceThread!=NULL) {
                zactor_destroy(&_deviceThread);
                zclock_sleep(1000);
                _deviceThread = NULL;
            }
        }

        void DeviceThread::InitInsides(zpoller_t *poller) {

            // Add insider ports
            for (auto&& it_insconf = _deviceConfig.component_ports.inss.begin();
                 it_insconf != _deviceConfig.component_ports.inss.end();
                 it_insconf++){

                auto newPortPtr = new ports::InsidePort(*it_insconf, NULL);
                std::unique_ptr<ports::PortBase> newport(newPortPtr);
                _insidePorts[it_insconf->portName] = std::move(newport);

                zpoller_add(poller, (void*)newPortPtr->GetSocket());
            }
        }

        DeviceThread::~DeviceThread() {
           StopThread();
        }

        void devThreadActor(zsock_t* pipe, void* args){
            DeviceThread* devThreadObj = (DeviceThread*) args;


            zpoller_t* poller = zpoller_new(pipe, NULL);
            assert(poller);
            zpoller_ignore_interrupts (poller);

            devThreadObj->InitInsides(poller);

            zsock_signal (pipe, 0);
            bool terminated = false;
            while (!terminated) {
                void *which = zpoller_wait(poller, 500);
                if (which == pipe) {
                    zmsg_t *msg = zmsg_recv(which);
                    if (!msg) {
                        std::cout << "No msg => interrupted" << std::endl;
                        break;
                    }

                    char *command = zmsg_popstr(msg);

                    if (streq(command, "$TERM")) {
                        std::cout << "$TERM arrived in component" << std::endl;
                        terminated = true;
                    }
                }
            }
        }
    }
}
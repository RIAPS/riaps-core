//
// Created by istvan on 6/20/17.
//

#include <IODeviceThread.h>

namespace echoio{
    namespace components{
        IODeviceThread::IODeviceThread(const _component_conf_j &deviceConfig)
            : DeviceThread(deviceConfig) {
            _consoleRep = zsock_new(ZMQ_REP);
            zsock_bind(_consoleRep, "tcp://127.0.0.1:5556");
            AddSocketToPoller(_consoleRep);
        }

        void IODeviceThread::Run() {
            std::cout << "DevThread::Run()" <<std::endl;

            InitInsides();

            riaps::ports::InsidePort* triggerPort = GetInsidePortByName(INSIDE_TRIGGER);

            if (triggerPort == NULL){
                throw std::invalid_argument("cannot find insideport");
            }

            zsock_t* triggerSocket = (zsock_t*)triggerPort->GetSocket();

            while (!IsTerminated()) {
                void* port = PollDeviceThreadPorts(100);

                if (port == triggerSocket){
                    zmsg_t* msg = zmsg_recv(port);
                    zmsg_send(&msg, _consoleRep);
                } else if (port == _consoleRep){
                    zmsg_t* msg = zmsg_recv(port);
                    SendTrigger(&msg);

                }
            }
        }

        bool IODeviceThread::SendTrigger(zmsg_t** message) {
            return SendMessageOnPort(message, INSIDE_TRIGGER);
        }

        IODeviceThread::~IODeviceThread() {
            Terminate();
            zclock_sleep(100);
        }
    }
}
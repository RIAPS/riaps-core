//
// Created by istvan on 6/20/17.
//

#ifndef RIAPS_CORE_IODEVICETHREAD_H
#define RIAPS_CORE_IODEVICETHREAD_H

#include "base/IODeviceBase.h"
#include <componentmodel/r_devicethread.h>
#include <componentmodel/r_insiderport.h>
#include <atomic>
#include <thread>

namespace echoio {
    namespace components {
        class IODeviceThread : public riaps::components::DeviceThread {
        public:
            IODeviceThread(const _component_conf_j& deviceConfig);

            void Run();

            bool SendTrigger(zmsg_t** zmsg);



            ~IODeviceThread();

        private:
            zsock_t* _consoleRep;

        };
    }
}

#endif //RIAPS_CORE_IODEVICETHREAD_H

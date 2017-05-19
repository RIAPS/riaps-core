//
// Created by istvan on 5/17/17.
//

#ifndef RIAPS_CORE_GPIODEVICETHREAD_H
#define RIAPS_CORE_GPIODEVICETHREAD_H

#include "base/GpioDeviceComponentBase.h"
#include <componentmodel/r_devicethread.h>
#include <componentmodel/r_insiderport.h>
#include <atomic>
#include <thread>

namespace gpiotoggleexample {
    namespace components {
        class GpioDeviceThread : public riaps::components::DeviceThread {
        public:
            GpioDeviceThread(const _component_conf_j& deviceConfig);

            void Run();

            bool IsGpioAvailable() const;
            void EnableGpio();
            void DisableGpio();
            void RequestGpioWrite();
            void RequestGpioRead();
            void RequestEdgeTrigger();


            ~GpioDeviceThread();

        private:
            std::atomic<bool> _isAvailable;
            std::atomic<bool> _readRequest;
            std::atomic<bool> _writeRequest;
            std::atomic<bool> _edgeTriggerEnable;

            std::string _currentValue;
        };
    }
}
#endif //RIAPS_CORE_GPIODEVICETHREAD_H

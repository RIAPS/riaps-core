//
// Created by istvan on 5/17/17.
//

#ifndef RIAPS_CORE_GPIODEVICETHREAD_H
#define RIAPS_CORE_GPIODEVICETHREAD_H

#include <base/Common.h>
#include "base/GpioDeviceComponentBase.h"
#include <componentmodel/r_devicethread.h>
#include <componentmodel/ports/r_insideport.h>
#include <spdlog/spdlog.h>
#include <atomic>
#include <thread>

namespace spd = spdlog;

namespace gpiotoggleexample {
    namespace components {
        class GpioDeviceThread : public riaps::components::DeviceThread {
        public:
            GpioDeviceThread(const _component_conf& deviceConfig);

            void Run();

            bool IsGpioAvailable() const;
            void EnableGpio();
            void DisableGpio();

            ~GpioDeviceThread();

        private:
            std::atomic<bool> _isAvailable;

            std::string _currentValue;


        };
    }
}
#endif //RIAPS_CORE_GPIODEVICETHREAD_H

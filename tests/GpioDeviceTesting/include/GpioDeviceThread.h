//
// Created by istvan on 5/17/17.
//

#ifndef RIAPS_CORE_GPIODEVICETHREAD_H
#define RIAPS_CORE_GPIODEVICETHREAD_H

#include <componentmodel/r_devicethread.h>
namespace riaps {
    namespace components {
        class GpioDeviceThread : public DeviceThread {
        public:
            GpioDeviceThread();

            bool IsGpioAvailable() const;

            ~GpioDeviceThread();

        private:
            bool _isAvailable;
        };
    }
}
#endif //RIAPS_CORE_GPIODEVICETHREAD_H

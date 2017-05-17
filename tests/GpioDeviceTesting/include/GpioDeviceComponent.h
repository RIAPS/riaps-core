//
// Created by istvan on 5/16/17.
//

#ifndef RIAPS_CORE_GPIODEVICECOMPONENT_H
#define RIAPS_CORE_GPIODEVICECOMPONENT_H


#include "base/GpioDeviceComponentBase.h"
#include <componentmodel/r_devicethread.h>

namespace gpiotoggleexample{
    namespace components{
        class GpioDeviceComponent : public GpioDeviceComponentBase{
        public:

            GpioDeviceComponent(_component_conf_j& config, riaps::Actor& actor);

            virtual void OnClock(riaps::ports::PortBase *port);

            virtual void OnReadGpio(const messages::ReadRequest::Reader &message,
                                    riaps::ports::PortBase *port);

            virtual void OnWriteGpio(const messages::WriteRequest::Reader &message,
                                     riaps::ports::PortBase *port);

            virtual void OnDataInQueue(const riaps::ports::InsideMessage::Reader &message,
                                       riaps::ports::PortBase *port);

            virtual void OnDataOutQueue(const riaps::ports::InsideMessage::Reader &message,
                                        riaps::ports::PortBase *port);

            void OnOneShotTimer(const std::string&);

            ~GpioDeviceComponent();

        private:
            __pid_t     _currentPid;
            std::string _bbbPinName;
            std::string _direction;
            std::string _pullUpDown;
            std::string _triggerEdge;
            uint8_t     _initialValue;
            uint32_t    _setupDelay;

            std::unique_ptr<riaps::components::DeviceThread> _deviceThread;

        };
    }
}


#endif //RIAPS_CORE_GPIODEVICECOMPONENT_H

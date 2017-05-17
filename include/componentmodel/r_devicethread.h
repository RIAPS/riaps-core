//
// Created by istvan on 5/17/17.
//

#ifndef RIAPS_CORE_R_DEVICETHREAD_H
#define RIAPS_CORE_R_DEVICETHREAD_H

#include <componentmodel/r_configuration.h>
#include <componentmodel/r_insiderport.h>
#include <czmq.h>
#include <string>

namespace riaps {
    namespace components {

        class DeviceThread {
        public:

            DeviceThread(_component_conf_j& deviceConfig);

            friend void devThreadActor(zsock_t *pipe, void *args);

            void StartThread();
            void StopThread();

            virtual ~DeviceThread();

        private:
            zactor_t *_deviceThread;
            _component_conf_j& _deviceConfig;
            std::map<std::string, std::unique_ptr<ports::PortBase>> _insidePorts;

            void InitInsides(zpoller_t* poller);
        };

        void devThreadActor(zsock_t *pipe, void *args);
    }
}

#endif //RIAPS_CORE_R_DEVICETHREAD_H

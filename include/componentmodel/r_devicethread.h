//
// Created by istvan on 5/17/17.
//

#ifndef RIAPS_CORE_R_DEVICETHREAD_H
#define RIAPS_CORE_R_DEVICETHREAD_H

#include <componentmodel/r_configuration.h>
#include <componentmodel/r_insiderport.h>
#include <czmq.h>
#include <string>
#include <thread>
#include <atomic>

namespace riaps {
    namespace components {

        /// \brief Base class for all device threads
        ///
        ///

        class DeviceThread {
        public:

            DeviceThread(const _component_conf_j& deviceConfig);

            //friend void devThreadActor(zsock_t *pipe, void *args);

            riaps::ports::InsidePort* GetInsidePortByName(const std::string& portName);


            virtual void Run()=0;

            void StartThread();
            void Terminate();

            bool IsTerminated() const;

            virtual ~DeviceThread();

        protected:
            // Note: Commented out until we figure out de we really need poller in every case
            //void InitInsides(zpoller_t* poller= NULL);
            void InitInsides();

            // TODO: Remove duplicated functions
            // TODO: move it to parent classe. componentbase also inherits this functionality.
            bool SendMessageOnPort(capnp::MallocMessageBuilder& message, const std::string& portName);
            bool SendMessageOnPort(zmsg_t **message, const std::string &portName);

        private:
            //zactor_t *_deviceThread;
            std::thread           _deviceThread;
            const _component_conf_j&    _deviceConfig;
            std::map<std::string, std::unique_ptr<ports::PortBase>> _insidePorts;
            std::atomic<bool> _isTerminated;

            zpoller_t* _poller;
        };

        //void devThreadActor(zsock_t *pipe, void *args);
    }
}

#endif //RIAPS_CORE_R_DEVICETHREAD_H

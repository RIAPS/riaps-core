//
// Created by istvan on 5/17/17.
//

#ifndef RIAPS_CORE_R_DEVICETHREAD_H
#define RIAPS_CORE_R_DEVICETHREAD_H

#include <componentmodel/r_configuration.h>
#include <componentmodel/ports/r_insideport.h>
#include <utils/r_utils.h>

#include <czmq.h>
#include <string>
#include <thread>
#include <atomic>

namespace riaps {
    namespace components {

        /**
         * Base class for all device threads
         */
        class DeviceThread {
        public:

            DeviceThread(const ComponentConf& deviceConfig);

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

            /**
             * Polls the registered inside ports for input data.
             * @param timeout Poller timeout in msec.
             * @return ZMQ socket, where the input data is waiting for recv() call.
             */
            void* PollDeviceThreadPorts(int timeout);

            void AddSocketToPoller(const zsock_t* socket);

            // TODO: Remove duplicated functions
            // TODO: move it to parent classe. componentbase also inherits this functionality.
            bool SendMessageOnPort(capnp::MallocMessageBuilder& message, const std::string& portName);
            //bool SendMessageOnPort(zmsg_t **message, const std::string &portName);

            std::shared_ptr<spdlog::logger> _logger;

        private:
            std::thread                 _deviceThread;
            const ComponentConf&    _deviceConfig;
            std::map<std::string, std::unique_ptr<ports::PortBase>> _insidePorts;
            std::atomic<bool> _isTerminated;

            zpoller_t* _poller;
        };
    }
}

#endif //RIAPS_CORE_R_DEVICETHREAD_H

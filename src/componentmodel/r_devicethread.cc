//
// Created by istvan on 5/17/17.
//


#include <componentmodel/r_devicethread.h>
#include <iostream>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace riaps{
    namespace components{


        DeviceThread::DeviceThread(const ComponentConf& deviceConfig)
                : _deviceConfig(deviceConfig){
            _isTerminated.store(false);
            _poller = NULL;
            _logger = spd::stdout_color_mt("DeviceThread");
            spd::set_level(spdlog::level::debug);
        }

        // Starts the zactor
        // Inside ports are initialized by the actor
        void DeviceThread::StartThread() {
            _logger->debug("Starting device thread");
            _deviceThread = std::thread(&DeviceThread::Run, this);
        }

        bool DeviceThread::IsTerminated() const {
            return _isTerminated.load();
        }

        void DeviceThread::Terminate() {
            _logger->debug("Terminate called");
            _isTerminated.store(true);
            if (_deviceThread.joinable())
                _deviceThread.join();
        }

        void DeviceThread::InitInsides() {

            // Add insider ports
            for (auto it_insconf = _deviceConfig.component_ports.inss.begin();
                 it_insconf != _deviceConfig.component_ports.inss.end();
                 it_insconf++){

                auto newPortPtr = new ports::InsidePort(*it_insconf, riaps::ports::InsidePortMode::CONNECT, NULL);
                std::unique_ptr<ports::PortBase> newport(newPortPtr);
                _insidePorts[it_insconf->port_name] = std::move(newport);

                AddSocketToPoller(newPortPtr->port_socket());
            }
        }

        void* DeviceThread::PollDeviceThreadPorts(int timeout) {
            void* port = zpoller_wait(_poller, timeout);
            return port;
        }

        void DeviceThread::AddSocketToPoller(const zsock_t *socket) {
            if (_poller!=NULL) {
                zpoller_add(_poller, (void *) socket);
            }
            else{
                _poller = zpoller_new((void*)socket, nullptr);
            }
        }

        bool DeviceThread::SendMessageOnPort(capnp::MallocMessageBuilder& message, const std::string &portName) {
//            auto serializedMessage = capnp::messageToFlatArray(message);
//            zmsg_t* msg = zmsg_new();
//            auto bytes = serializedMessage.asBytes();
//            zmsg_pushmem(msg, bytes.begin(), bytes.size());
//            return SendMessageOnPort(&msg, portName);

            auto port = GetInsidePortByName(portName);
            if (port == NULL) return false;
            return port->Send(message);

        }

//        bool DeviceThread::SendMessageOnPort(zmsg_t **message, const std::string &portName) {
//            auto port = GetInsidePortByName(portName);
//            if (port == NULL) return false;
//            return port->Send(message);
//        }



        riaps::ports::InsidePort* DeviceThread::GetInsidePortByName(const std::string &portName) {
            if (_insidePorts.find(portName)!=_insidePorts.end()){
                return ((riaps::ports::InsidePort*)_insidePorts[portName].get());
            }
            return NULL;
        }

        DeviceThread::~DeviceThread() {
            for(auto it = _insidePorts.begin(); it!=_insidePorts.end(); it++){
                auto ptr = it->second.release();
                delete ptr;
            }
           Terminate();
        }


    }
}
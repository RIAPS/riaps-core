//
// Created by istvan on 5/17/17.
//

#include <GpioDeviceThread.h>

namespace gpiotoggleexample {
    namespace components {

        GpioDeviceThread::GpioDeviceThread(const _component_conf& deviceConfig)
            : riaps::components::DeviceThread(deviceConfig) {

            _isAvailable.store(false);

        }


        void GpioDeviceThread::Run() {
            _logger->debug("DeviceThread started.");

            // Inside ports are put in a poller automatically
            InitInsides();
            riaps::ports::InsidePort* dataPort = GetInsidePortByName(INSIDE_DATAQUEUE);

            if (dataPort == NULL){
                _logger->critical("cannot find insideport");
                return;
            }

            zsock_t* dataSocket = (zsock_t*)dataPort->GetSocket();

            EnableGpio();

            while (!IsTerminated()) {
                void* port = PollDeviceThreadPorts(100);

                if (port == dataSocket){
                    zmsg_t* msg = zmsg_recv(port);
                    char* command = zmsg_popstr(msg);

                    if (streq(command, CMD_WRITE_REQUEST)){
                        char* value = zmsg_popstr(msg);
                        _logger->debug("GPIO write request received, value is updated: {} -> {}",_currentValue,value);
                        _currentValue.assign(value);
                        zstr_free(&value);
                    } else if (streq(command, CMD_READ_REQUEST)){
                        _logger->debug("GPIO read request received.");
                        int rc = zsock_send(dataSocket, "s", _currentValue.c_str());
                        _logger->debug_if(rc==0,"Send successful");
                        _logger->error_if(rc==-1, "Send failed");
                    }
                    zstr_free(&command);
                    zmsg_destroy(&msg);
                }
            }
        }

        bool GpioDeviceThread::IsGpioAvailable() const {
            return _isAvailable.load();
        }


        void GpioDeviceThread::EnableGpio() {
            _logger->debug("GpioDeviceThread GPIO=%s setup and available for use"); //, self.component.bbb_pin_name)
            _isAvailable.store(true);
        }

        void GpioDeviceThread::DisableGpio() {
            _logger->debug("GpioDeviceThread - disabled GPIO: %s"); //,self.component.bbb_pin_name)
            _isAvailable.store(false);
        }


        GpioDeviceThread::~GpioDeviceThread() {

        }

    }
}
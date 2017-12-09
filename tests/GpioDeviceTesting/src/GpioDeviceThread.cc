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
                throw std::invalid_argument("cannot find insideport");
            }

            zsock_t* dataSocket = (zsock_t*)dataPort->GetSocket();

            EnableGpio();

            while (!IsTerminated()) {
                void* port = PollDeviceThreadPorts(100);

                if (port == dataSocket){
                    zmsg_t* msg = zmsg_recv(port);
                    char* command = zmsg_popstr(msg);

                    if (streq(command, CMD_WRITE_REQUEST)){
                        _logger->debug("GPIO write request received.");
                        char* value = zmsg_popstr(msg);
                        _currentValue.assign(value);
                        zstr_free(&value);
                    } else if (streq(command, CMD_READ_REQUEST)){
                        _logger->debug("GPIO read request received.");
                        zsock_send(dataPort, "s", _currentValue.c_str());
                        //_logger->debug("GPIO data sent");
                    }
//
//                    if (streq(command, CMD_WRITE_REQUEST)){
//                        _logger->debug("GPIO write request received.");
//                        char* value = zmsg_popstr(msg);
//                        _currentValue = std::string(value);
//                        zstr_free(&value);
//                    } else if (streq(command, CMD_READ_REQUEST)){
//                        _logger->debug("GPIO read request received.");
//                        //zmsg_t* response = zmsg_new();
//                        //zmsg_addstr(response, _currentValue.c_str());
//
//                        zsock_send(dataPort, "s", _currentValue.c_str());
//                        _logger->debug("data sent");
//                        //std::cout << "GpioDeviceThread::ReadRequest value " << _currentValue << std::endl;
//                    }

                    zstr_free(&command);
                    zmsg_destroy(&msg);
                }
            }


        }

        bool GpioDeviceThread::IsGpioAvailable() const {
            return _isAvailable.load();
        }


        void GpioDeviceThread::EnableGpio() {
            //std ::cout << "GpioDeviceThread setting up GPIO=%s: direction=%s resistor=%s trigger=%s ivalue=%d delay=%d [%d]", self.component.bbb_pin_name, self.component.direction, self.component.pull_up_down, self.component.trigger_edge, self.component.initial_value, self.component.setup_delay, self.pid)
            //GPIO.setup(self.component.bbb_pin_name, self.direction, self.pull_up_down, self.component.initial_value, self.component.setup_delay)
            std::cout << "GpioDeviceThread GPIO=%s setup and available for use" <<std::endl; //, self.component.bbb_pin_name)
            _isAvailable.store(true);
        }

        void GpioDeviceThread::DisableGpio() {
            std::cout << "GpioDeviceThread - disabled GPIO: %s" << std::endl; //,self.component.bbb_pin_name)
            _isAvailable.store(false);
        }


        GpioDeviceThread::~GpioDeviceThread() {

        }

    }
}
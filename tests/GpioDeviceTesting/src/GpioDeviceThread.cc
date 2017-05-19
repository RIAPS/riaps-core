//
// Created by istvan on 5/17/17.
//

#include <GpioDeviceThread.h>

namespace gpiotoggleexample {
    namespace components {



        GpioDeviceThread::GpioDeviceThread(const _component_conf_j& deviceConfig)
            : riaps::components::DeviceThread(deviceConfig) {

            _isAvailable.store(false);
            _readRequest.store(false);
            _writeRequest.store(false);
            _edgeTriggerEnable.store(false);
        }


        void GpioDeviceThread::Run() {
            InitInsides();

            riaps::ports::InsidePort* dataOutPort = GetInsidePortByName(INSIDE_DATAOUT_QUEUE);
            riaps::ports::InsidePort* dataInPort  = GetInsidePortByName(INSIDE_DATAIN_QUEUE);

            if (dataInPort == NULL || dataOutPort == NULL){
                throw std::invalid_argument("cannot find insideport");
            }

            EnableGpio();

            while (!IsTerminated()){

                if (_writeRequest.load()){
                    riaps::ports::InsideMessage::Reader* insideMessage = NULL;
                    auto result = dataOutPort->Recv(&insideMessage);

                    if (result && insideMessage!=NULL) {
                        _currentValue = insideMessage->getValue();
                    }

                    _writeRequest.store(false);
                }
                else {
                    if (_readRequest.load()) {
                        std::cout << "read" << std::endl;
                        capnp::MallocMessageBuilder builder;
                        auto insideMsg = builder.initRoot<riaps::ports::InsideMessage>();
                        insideMsg.setValue(_currentValue);
                        std::cout << "ReadRequest, current value: " << _currentValue << std::endl;


                        auto result =  SendMessageOnPort(builder, INSIDE_DATAIN_QUEUE);
                        std::cout << "Send result: " << result <<std::endl;

                        _readRequest.store(false);
                    }
                }
//                else if (_edgeTriggerEnable.load()){
//                    _edgeTriggerEnable.store(false);
//                }
            }
        }

        bool GpioDeviceThread::IsGpioAvailable() const {
            return _isAvailable.load();
        }

        void GpioDeviceThread::RequestGpioRead() {
            _readRequest.store(true);
            std::cout << "GpioDeviceThread - read request received" << std::endl;
        }

        void GpioDeviceThread::RequestGpioWrite() {
            _writeRequest.store(true);
            std::cout << "GpioDeviceThread - write request received" << std::endl;
        }

        void GpioDeviceThread::RequestEdgeTrigger() {
            std::cout << "GpioDeviceThread - edge triggering request received" << std::endl;
        }

        void GpioDeviceThread::EnableGpio() {
            //std ::cout << "GpioDeviceThread setting up GPIO=%s: direction=%s resistor=%s trigger=%s ivalue=%d delay=%d [%d]", self.component.bbb_pin_name, self.component.direction, self.component.pull_up_down, self.component.trigger_edge, self.component.initial_value, self.component.setup_delay, self.pid)
            //GPIO.setup(self.component.bbb_pin_name, self.direction, self.pull_up_down, self.component.initial_value, self.component.setup_delay)
            std::cout << "GpioDeviceThread GPIO=%s setup and available for use" <<std::endl; //, self.component.bbb_pin_name)
            _isAvailable.store(true);
        }

        void GpioDeviceThread::DisableGpio() {
            std::cout << "GpioDeviceThread - disabled GPIO: %s" << std::endl; //,self.component.bbb_pin_name)
        }


        GpioDeviceThread::~GpioDeviceThread() {

        }

    }
}
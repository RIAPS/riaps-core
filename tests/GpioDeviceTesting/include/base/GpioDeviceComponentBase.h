//
// Created by istvan on 5/16/17.
//

#ifndef RIAPS_CORE_GPIODEVICECOMPONENTBASE_H
#define RIAPS_CORE_GPIODEVICECOMPONENTBASE_H

#include "messages/GpioToggleExample.capnp.h"
#include <componentmodel/r_componentbase.h>

#define TIMER_CLOCK "clock"
#define PORT_PUB_REPORTEDDATA "reportedData"
#define PORT_SUB_READGPIO "readGpio"
#define PORT_SUB_WRITEGPIO "writeGpio"
#define INSIDE_DATAIN_QUEUE "dataIn_queue"
#define INSIDE_DATAOUT_QUEUE "dataOut_queue"

namespace gpiotoggleexample {
    namespace components {

        class GpioDeviceComponentBase : public riaps::ComponentBase {
        public:
            GpioDeviceComponentBase(_component_conf_j &config, riaps::Actor &actor);

            virtual void OnClock(riaps::ports::PortBase *port)=0;

            virtual void OnReadGpio(const messages::ReadRequest::Reader &message,
                                    riaps::ports::PortBase *port)=0;

            virtual void OnWriteGpio(const messages::WriteRequest::Reader &message,
                                    riaps::ports::PortBase *port)=0;

            virtual bool SendReportedData(capnp::MallocMessageBuilder&    messageBuilder,
                                          messages::DataValue::Builder& message);

            virtual ~GpioDeviceComponentBase();

        protected:

            virtual void DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                         riaps::ports::PortBase *port);
        };
    }
}

#endif //RIAPS_CORE_GPIODEVICECOMPONENTBASE_H

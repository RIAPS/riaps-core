//
// Created by istvan on 5/16/17.
//

#ifndef RIAPS_CORE_TOGGLEGPIOCOMPONENTBASE_H
#define RIAPS_CORE_TOGGLEGPIOCOMPONENTBASE_H


#include <base/Common.h>
#include <componentmodel/r_actor.h>
#include "messages/GpioToggleExample.capnp.h"

#define PORT_PUB_POLLGPIOVAPUE "pollGpioValue"	     // Publish request to read the specific GPIO value
#define PORT_PUB_WRITEGPIOVALUE "writeGpioValue"     // Publish request to write a value to the specific GPIO
#define PORT_SUB_CURRENTGPIOVALUE "currentGpioValue" // Receive the value of the specific GPIO
#define TIMER_TOGGLE "toggle"                        // Toggle GPIO value
#define TIMER_READVALUE "readValue"                  // Read current GPIO value

namespace gpiotoggleexample {
    namespace components {

        class ToggleGpioComponentBase : public riaps::ComponentBase {

        public:

            ToggleGpioComponentBase(_component_conf &config, riaps::Actor &actor);

            virtual void OnToggle(riaps::ports::PortBase *port)=0;
            virtual void OnReadValue(riaps::ports::PortBase *port)=0;

            virtual void OnCurrentGpioValue(const messages::DataValue::Reader &message,
                                            riaps::ports::PortBase *port)=0;

            virtual bool SendPollGpioValue(capnp::MallocMessageBuilder&    messageBuilder,
                                           messages::ReadRequest::Builder& message);

            virtual bool SendWriteGpioValue(capnp::MallocMessageBuilder&    messageBuilder,
                                            messages::WriteRequest::Builder& message);


            virtual ~ToggleGpioComponentBase();

        protected:

            virtual void DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                         riaps::ports::PortBase *port,
                                         std::shared_ptr<riaps::MessageParams> params);

            virtual void DispatchInsideMessage(zmsg_t*, riaps::ports::PortBase*);


        };
    }
}

#endif //RIAPS_CORE_TOGGLEGPIOCOMPONENTBASE_H

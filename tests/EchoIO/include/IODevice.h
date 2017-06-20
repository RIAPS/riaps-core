//
// Created by istvan on 6/20/17.
//

#ifndef RIAPS_CORE_IODEVICE_H
#define RIAPS_CORE_IODEVICE_H

#include <base/IODeviceBase.h>

namespace echoio {
    namespace components {
        class IODevice : public IODeviceBase{
        public:

            IODevice(_component_conf_j &config, riaps::Actor &actor);

            void OnClock(riaps::ports::PortBase *port);

            void OnEcho(const messages::EchoRep::Reader &message,
                                riaps::ports::PortBase *port);

            void OnOneShotTimer(const std::string&);


            ~IODevice();
        };
    }
}

extern "C" riaps::ComponentBase* create_component(_component_conf_j&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);



#endif //RIAPS_CORE_IODEVICE_H

//
// Created by istvan on 6/20/17.
//

#ifndef RIAPS_CORE_ECHO_H
#define RIAPS_CORE_ECHO_H

#include "base/EchoBase.h"

namespace echoio{
    namespace components{
        class Echo : public EchoBase{
        public:
            Echo(_component_conf_j &config, riaps::Actor &actor);

            void OnClock(riaps::ports::PortBase *port);

            void OnEcho(const messages::EchoReq::Reader &message,
                                riaps::ports::PortBase *port);

            void OnOneShotTimer(const std::string& timerid);

            ~Echo();

        private:
            void reverseString(const std::string& original, std::string& reversed);
        };
    }
}

extern "C" riaps::ComponentBase* create_component(_component_conf_j&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);

#endif //RIAPS_CORE_ECHO_H

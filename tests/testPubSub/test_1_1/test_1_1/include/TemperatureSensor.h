//
// Created by istvan on 2/28/17.
//

#ifndef RIAPS_CORE_TEMPERATURESENSOR_H
#define RIAPS_CORE_TEMPERATURESENSOR_H

#include <base/TemperatureSensorBase.h>
#include "common.h"

#include <fstream>

namespace testing{
    namespace components{
        class TemperatureSensor : public testing::components::TemperatureSensorBase{
        public:
            TemperatureSensor(_component_conf_j &config, riaps::Actor &actor);

            virtual void OnClock(riaps::ports::PortBase *port);

            virtual ~TemperatureSensor();

        protected:
            std::string _logfilePath;
            std::fstream _logStream;
            int _messageCounter;
        };
    }
}

extern "C" riaps::ComponentBase* create_component(_component_conf_j&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);

#endif //RIAPS_CORE_TEMPERATURESENSOR_H

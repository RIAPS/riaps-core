//
// Created by istvan on 2/28/17.
//

#ifndef RIAPS_CORE_COLLECTOR_H_H
#define RIAPS_CORE_COLLECTOR_H_H

#include "common.h"
#include <base/CollectorBase.h>

#include <fstream>

namespace testing {
    namespace components {

        class Collector : public testing::components::CollectorBase {

        public:

            Collector(_component_conf_j &config, riaps::Actor &actor);

            virtual void OnGetTemperature(testing::messages::SensorValue::Reader& message,
                                          riaps::ports::PortBase* port);
										  
			virtual void OnOneShotTimer(const std::string& timerid);						  

            virtual ~Collector();

        protected:
            std::string _logfilePath;
            std::fstream _logStream;
            int _messageCounter;
        };
    }
}

extern "C" riaps::ComponentBase* create_component(_component_conf_j&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);

#endif //RIAPS_CORE_COLLECTOR_H_H

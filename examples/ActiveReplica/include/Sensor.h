//
// Created by istvan on 11/11/16.
//

#ifndef RIAPS_FW_SENSOR_H
#define RIAPS_FW_SENSOR_H

#include "base/SensorBase.h"

namespace activereplica {
    namespace components {

        class Sensor : public SensorBase {

        public:

            Sensor(_component_conf &config, riaps::Actor &actor);
            virtual ~Sensor();

        protected:

            virtual void OnClock(riaps::ports::PortBase *port);



            virtual void OnRequest(const messages::SensorQuery::Reader &message,
                                   riaps::ports::PortBase *port,
                                   std::shared_ptr<riaps::MessageParams> params = nullptr);
            
        private:
            std::uniform_real_distribution<double> _rndDistr;
            std::default_random_engine             _rndEngine;
        };
    }
}

extern "C" riaps::ComponentBase* create_component(_component_conf&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);


#endif //RIAPS_FW_SENSOR_H

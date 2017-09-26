//
// Created by istvan on 11/11/16.
//

#ifndef RIAPS_FW_SENSOR_H
#define RIAPS_FW_SENSOR_H

#include "base/SensorBase.h"

namespace distributedestimator {
    namespace components {

        class comp_sensor : public comp_sensorbase {

        public:

            comp_sensor(_component_conf &config, riaps::Actor &actor);


            virtual void OnClock(riaps::ports::PortBase *port);

            virtual void OnRequest(const messages::SensorQuery::Reader &message,
                                   riaps::ports::PortBase *port);

            virtual void OnGroupMessage(riaps::groups::GroupId& groupId,
                                        capnp::FlatArrayMessageReader* capnpreader);

            virtual bool SendGroupMessage(riaps::groups::GroupId&      groupId,
                                          capnp::MallocMessageBuilder& messageBuilder,
                                          const std::string&           portName);


            virtual ~comp_sensor();


        private:
            std::unique_ptr<std::uniform_real_distribution<double>> unif;
            std::default_random_engine re;
        };
    }
}

extern "C" riaps::ComponentBase* create_component(_component_conf&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);


#endif //RIAPS_FW_SENSOR_H

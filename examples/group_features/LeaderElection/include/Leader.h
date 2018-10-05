//
// Created by istvan on 11/11/16.
//

#ifndef RIAPS_FW_SENSOR_H
#define RIAPS_FW_SENSOR_H

#include "base/LeaderBase.h"

namespace leaderelection {
    namespace components {

        class Leader : public LeaderBase {

        public:

            Leader(_component_conf &config, riaps::Actor &actor);
            virtual ~Leader();

        protected:

            virtual void OnClock(riaps::ports::PortBase *port);

            void OnGroupMessage(const riaps::groups::GroupId& groupId,
                                capnp::FlatArrayMessageReader& capnpreader,
                                riaps::ports::PortBase* port);

            virtual bool SendGroupMessage(riaps::groups::GroupId&      groupId,
                                          capnp::MallocMessageBuilder& messageBuilder,
                                          const std::string&           portName);
        private:
            bool _hasJoined;

            std::unique_ptr<std::uniform_real_distribution<double>> unif;
            std::default_random_engine re;
        };
    }
}

extern "C" riaps::ComponentBase* create_component(_component_conf&, riaps::Actor& actor);
extern "C" void destroy_component(riaps::ComponentBase*);


#endif //RIAPS_FW_SENSOR_H

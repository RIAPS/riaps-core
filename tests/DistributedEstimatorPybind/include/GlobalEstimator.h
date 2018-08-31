//
// Created by istvan on 11/11/16.
//

#ifndef RIAPS_FW_GLOBALESTIMATOR_H
#define RIAPS_FW_GLOBALESTIMATOR_H

#include "base/GlobalEstimatorBase.h"

namespace distributedestimator {
    namespace components {

        class GlobalEstimator : public GlobalEstimatorBase {

        public:

            //GlobalEstimator(_component_conf &config, riaps::Actor &actor);
            
            GlobalEstimator(const py::object *parent_actor,
                            const py::dict actor_spec, // Actor json config
                            const py::dict type_spec,  // component json config
                            const std::string &name,
                            const std::string &type_name,
                            const py::dict args,
                            const std::string &application_name,
                            const std::string &actor_name);

            virtual void OnEstimate(messages::Estimate::Reader &message,
                                    riaps::ports::PortBase *port) override;

            virtual void OnWakeup(riaps::ports::PortBase *port) override;


            virtual ~GlobalEstimator();

        private:
            std::unique_ptr<std::uniform_real_distribution<double>> unif;
            std::default_random_engine re;
        };
    }
}
//
//extern "C" riaps::ComponentBase* create_component(_component_conf&, riaps::Actor& actor);
//extern "C" void destroy_component(riaps::ComponentBase*);

std::unique_ptr<distributedestimator::components::GlobalEstimator>
create_component_py(const py::object *parent_actor,
                    const py::dict actor_spec,
                    const py::dict type_spec,
                    const std::string &name,
                    const std::string &type_name,
                    const py::dict args,
                    const std::string &application_name,
                    const std::string &actor_name);

#endif //RIAPS_FW_GLOBALESTIMATOR_H

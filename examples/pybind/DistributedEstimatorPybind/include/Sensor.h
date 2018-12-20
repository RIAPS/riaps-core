//
// Created by istvan on 11/11/16.
//

#ifndef RIAPS_FW_SENSOR_H
#define RIAPS_FW_SENSOR_H

#include "base/SensorBase.h"

namespace distributedestimator {
    namespace components {

        class Sensor : public SensorBase {

        public:

            Sensor(const py::object *parent_actor,
                   const py::dict actor_spec, // Actor json config
                   const py::dict type_spec,  // component json config
                   const std::string &name,
                   const std::string &type_name,
                   const py::dict args,
                   const std::string &application_name,
                   const std::string &actor_name);

            Sensor(const Sensor&) = delete;
            Sensor()              = delete;

            virtual void OnClock() override;
            virtual void OnRequest() override;

            virtual ~Sensor();
        private:
            std::unique_ptr<std::uniform_real_distribution<double>> unif;
            std::default_random_engine re;
        };
    }
}

//extern "C" riaps::ComponentBase* create_component(_component_conf&, riaps::Actor& actor);
//extern "C" void destroy_component(riaps::ComponentBase*);


#endif //RIAPS_FW_SENSOR_H

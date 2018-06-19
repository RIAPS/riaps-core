//
// Created by istvan on 3/10/17.
//

#ifndef RIAPS_CORE_GLOBALESTIMATORBASE_H
#define RIAPS_CORE_GLOBALESTIMATORBASE_H

#include "componentmodel/r_componentbase.h"
#include "messages/distributedestimator.capnp.h"

#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

#define PORT_SUB_ESTIMATE "estimate"
#define PORT_TIMER_WAKEUP "wakeup"

namespace distributedestimator{
    namespace components{
        class GlobalEstimatorBase : public riaps::ComponentBase {

        public:

            GlobalEstimatorBase(_component_conf& config, riaps::Actor& actor);

            GlobalEstimatorBase(const py::object*  parent_actor ,
                                const py::dict     type_spec    ,
                                const std::string& name         ,
                                const std::string& type_name    ,
                                const py::dict     args);

            virtual void OnEstimate(messages::Estimate::Reader& message,
                                    riaps::ports::PortBase* port)=0;

            virtual void OnWakeup(riaps::ports::PortBase* port)=0;

            template<class T>
            void printNames(std::vector<T>& ports){
                auto kkk = spd::get("debuglogger");
                for (auto& item : ports) {
                    kkk->info("port: {}", item.portName);
                }
            };

            virtual ~GlobalEstimatorBase();

        protected:
            virtual void DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                         riaps::ports::PortBase*   port,
                                         std::shared_ptr<riaps::MessageParams> params);

            virtual void DispatchInsideMessage(zmsg_t* zmsg,
                                               riaps::ports::PortBase* port);


        private:
            std::unique_ptr<std::uniform_real_distribution<double>> unif;
            std::default_random_engine                              re;
        };
    }
}


#endif //RIAPS_CORE_GLOBALESTIMATORBASE_H

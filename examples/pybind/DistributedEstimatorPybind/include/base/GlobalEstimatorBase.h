//
// Created by istvan on 3/10/17.
//

#ifndef RIAPS_CORE_GLOBALESTIMATORBASE_H
#define RIAPS_CORE_GLOBALESTIMATORBASE_H

#include "componentmodel/r_componentbase.h"
#include "messages/distributedestimator.capnp.h"
#include "componentmodel/r_messagebuilder.h"
#include <componentmodel/r_messagereader.h>

#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <boost/optional.hpp>

namespace py = pybind11;

constexpr auto PORT_SUB_ESTIMATE = "estimate";
constexpr auto PORT_TIMER_WAKEUP = "wakeup";

namespace distributedestimator{
    namespace components{
        class GlobalEstimatorBase : public riaps::ComponentBase {

        public:
            GlobalEstimatorBase(const py::object *parent_actor,
                                const py::dict actor_spec, // Actor json config
                                const py::dict type_spec,  // component json config
                                const std::string &name,
                                const std::string &type_name,
                                const py::dict args,
                                const std::string &application_name,
                                const std::string &actor_name);

            virtual void OnEstimate()=0;
            virtual void OnWakeup()=0;

            virtual std::tuple<MessageReader<messages::Estimate>, riaps::ports::PortError> RecvEstimate() final;
            virtual timespec RecvWakeup() final;

            virtual ~GlobalEstimatorBase() = default;

        protected:
            virtual void DispatchMessage(riaps::ports::PortBase* port) final;

            virtual void DispatchInsideMessage(zmsg_t* zmsg,
                                               riaps::ports::PortBase* port) final;

        };
    }
}


#endif //RIAPS_CORE_GLOBALESTIMATORBASE_H

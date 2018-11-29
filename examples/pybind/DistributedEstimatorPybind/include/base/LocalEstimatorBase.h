//
// Created by istvan on 3/10/17.
//

#ifndef RIAPS_CORE_LOCALESTIMATORBASE_H
#define RIAPS_CORE_LOCALESTIMATORBASE_H

#include "componentmodel/r_componentbase.h"
#include "messages/distributedestimator.capnp.h"
#include "componentmodel/r_messagebuilder.h"
#include <componentmodel/r_messagereader.h>


#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

// Name of the ports from the model file
constexpr auto PORT_SUB_READY    = "ready";
constexpr auto PORT_REQ_QUERY    = "query";
constexpr auto PORT_PUB_ESTIMATE = "estimate";

namespace distributedestimator {
    namespace components {

        class LocalEstimatorBase : public riaps::ComponentBase {

        public:

            LocalEstimatorBase(const py::object *parent_actor,
                               const py::dict actor_spec, // Actor json config
                               const py::dict type_spec,  // component json config
                               const std::string &name,
                               const std::string &type_name,
                               const py::dict args,
                               const std::string &application_name,
                               const std::string &actor_name);

            virtual void OnReady()=0;
            virtual void OnQuery() = 0;

            virtual std::tuple<MessageReader<messages::SensorReady>, riaps::ports::PortError> RecvReady() final;
            virtual std::tuple<MessageReader<messages::SensorValue>, riaps::ports::PortError> RecvQuery() final;

            riaps::ports::PortError SendEstimate(MessageBuilder<messages::Estimate>& message);
            riaps::ports::PortError SendQuery(MessageBuilder<messages::SensorQuery>& message);

            virtual ~LocalEstimatorBase() = default;

        protected:
            virtual void DispatchMessage(riaps::ports::PortBase *port) final;

            virtual void DispatchInsideMessage(zmsg_t* zmsg,
                                               riaps::ports::PortBase* port) final;

        };
    }
}

#endif //RIAPS_CORE_LOCALESTIMATORBASE_H

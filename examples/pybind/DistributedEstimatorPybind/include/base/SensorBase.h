//
// Created by istvan on 3/10/17.
//

#ifndef RIAPS_CORE_SENSORBASE_H
#define RIAPS_CORE_SENSORBASE_H

#include "componentmodel/r_componentbase.h"
#include "messages/distributedestimator.capnp.h"
#include "componentmodel/r_messagebuilder.h"
#include <componentmodel/r_messagereader.h>

#include <pybind11/stl.h>
#include <pybind11/pybind11.h>


namespace py = pybind11;

// Name of the ports from the model file
constexpr auto PORT_TIMER_CLOCK = "clock";
constexpr auto PORT_PUB_READY   = "ready";
constexpr auto PORT_REP_REQUEST = "request";

namespace distributedestimator {
    namespace components {

        class SensorBase : public riaps::ComponentBase {

        public:

            SensorBase(const py::object *parent_actor,
                       const py::dict actor_spec,
                       const py::dict type_spec,
                       const std::string &name,
                       const std::string &type_name,
                       const py::dict args,
                       const std::string &application_name,
                       const std::string &actor_name);

            virtual void OnClock()=0;
            virtual void OnRequest()=0;

            virtual timespec RecvClock() final;
            virtual std::tuple<MessageReader<messages::SensorQuery>, riaps::ports::PortError> RecvRequest() final;

            virtual riaps::ports::PortError SendRequest(MessageBuilder<messages::SensorValue>& message);
            virtual riaps::ports::PortError SendReady(MessageBuilder<messages::SensorReady>& builder);

            virtual ~SensorBase() = default;

        protected:

            virtual void DispatchMessage(riaps::ports::PortBase *port) final;
            virtual void DispatchInsideMessage(zmsg_t* zmsg,
                                               riaps::ports::PortBase* port) final;
        };
    }
}

#endif //RIAPS_CORE_SENSORBASE_H

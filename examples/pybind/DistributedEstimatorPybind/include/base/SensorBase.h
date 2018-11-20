//
// Created by istvan on 3/10/17.
//

#ifndef RIAPS_CORE_SENSORBASE_H
#define RIAPS_CORE_SENSORBASE_H

#include "componentmodel/r_componentbase.h"
#include "messages/distributedestimator.capnp.h"
#include "componentmodel/r_messagebuilder.h"

#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

// Name of the ports from the model file
#define PORT_TIMER_CLOCK "clock"
#define PORT_PUB_READY   "ready"
#define PORT_REP_REQUEST "request"

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

            virtual std::string RecvClock() final;
            virtual messages::SensorQuery::Reader RecvRequest() final;

            virtual bool SendRequest(MessageBuilder<messages::SensorValue>& message);
            virtual bool SendReady(MessageBuilder<messages::SensorReady>& builder);

            virtual ~SensorBase();

        protected:

            virtual void DispatchMessage(riaps::ports::PortBase *port) final;
            virtual void DispatchInsideMessage(zmsg_t* zmsg,
                                               riaps::ports::PortBase* port) final;
        };
    }
}

#endif //RIAPS_CORE_SENSORBASE_H

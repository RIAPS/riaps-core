//
// Created by istvan on 3/10/17.
//

#ifndef RIAPS_CORE_SENSORBASE_H
#define RIAPS_CORE_SENSORBASE_H

#include "componentmodel/r_componentbase.h"
#include "messages/distributedestimator.capnp.h"

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

            //comp_sensorbase(_component_conf &config, riaps::Actor &actor);

            SensorBase(const py::object *parent_actor,
                       const py::dict actor_spec, // Actor json config
                       const py::dict type_spec,  // component json config
                       const std::string &name,
                       const std::string &type_name,
                       const py::dict args,
                       const std::string &application_name,
                       const std::string &actor_name);

            //virtual void RegisterHandlers();

            virtual void OnClock(riaps::ports::PortBase *port)=0;

            // No handler for publisher
            // But send function for publisher, request, response makes sense, maybe easier for the developer
            //virtual void OnReady(const std::string& messagetype,
            //                     std::vector<std::string>& msgFields,
            //                     riaps::ports::PortBase* port)=0;

            virtual void OnRequest(const messages::SensorQuery::Reader &message,
                                   riaps::ports::PortBase *port)=0;

            virtual bool SendRequest(capnp::MallocMessageBuilder&    messageBuilder,
                                     messages::SensorValue::Builder& message);

            virtual bool SendReady(capnp::MallocMessageBuilder&    messageBuilder,
                                   messages::SensorReady::Builder& message);


            virtual ~SensorBase();

        protected:

            virtual void DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                         riaps::ports::PortBase *port,
                                         std::shared_ptr<riaps::MessageParams> params) final;

            virtual void DispatchInsideMessage(zmsg_t* zmsg,
                                               riaps::ports::PortBase* port) final;


        };
    }
}

#endif //RIAPS_CORE_SENSORBASE_H

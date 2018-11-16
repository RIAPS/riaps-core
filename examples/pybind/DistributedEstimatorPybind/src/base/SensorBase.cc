//
// Created by istvan on 3/10/17.
//

#include <base/SensorBase.h>
#include <componentmodel/r_pyconfigconverter.h>

using namespace std;

namespace distributedestimator {
    namespace components {

        SensorBase::SensorBase(const py::object *parent_actor,
                               const py::dict actor_spec, // Actor json config
                               const py::dict type_spec,  // component json config
                               const std::string &name,
                               const std::string &type_name,
                               const py::dict args,
                               const std::string &application_name,
                               const std::string &actor_name)
                : ComponentBase(application_name, actor_name) {
            auto config = PyConfigConverter::convert(type_spec, actor_spec);
            config.component_name = name;
            config.component_type = type_name;
            config.is_device=false;
            set_config(config);
            set_debug_level(spd::level::info);
        }

        string SensorBase::RecvClock() {
            auto port = GetPortAs<riaps::ports::PeriodicTimer>(PORT_TIMER_CLOCK);
            return port->Recv();
        }

        messages::SensorQuery::Reader SensorBase::RecvRequest() {
            auto port = GetPortAs<riaps::ports::ResponsePort>(PORT_REP_REQUEST);
            auto reader = port->Recv();
            return reader->getRoot<messages::SensorQuery>();

        }

        void SensorBase::DispatchMessage(riaps::ports::PortBase* port) {
            auto portName = port->port_name();
            if (portName == PORT_TIMER_CLOCK) {
                OnClock();
            } else if (portName == PORT_REP_REQUEST) {
                OnRequest();
            }

        }

        void SensorBase::DispatchInsideMessage(zmsg_t *zmsg, riaps::ports::PortBase *port) {

        }

        bool SensorBase::SendRequest(capnp::MallocMessageBuilder&    messageBuilder,
                                          messages::SensorValue::Builder& message) {
            return SendMessageOnPort(messageBuilder, PORT_REP_REQUEST);
        }

        bool SensorBase::SendReady(capnp::MallocMessageBuilder&    messageBuilder,
                                        messages::SensorReady::Builder& message) {
            return SendMessageOnPort(messageBuilder, PORT_PUB_READY);
        }

        SensorBase::~SensorBase() {

        }

    }
}
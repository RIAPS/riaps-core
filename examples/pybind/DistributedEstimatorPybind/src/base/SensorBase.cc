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
            config.isDevice=false;
            set_config(config);
        }

        string SensorBase::RecvClock() {
            component_logger()->debug("{}", __func__);
            auto port = (riaps::ports::PeriodicTimer*)GetPortByName(PORT_TIMER_CLOCK);
            component_logger()->debug("GetPortByName()");
            auto rport = port->AsRecvPort();
            component_logger()->debug("AsRecvPort()");
            return rport->RecvAsString();
        }

        messages::SensorQuery::Reader SensorBase::RecvRequest() {
            auto port = GetResponsePortByName(PORT_REP_REQUEST);
            auto reader = port->Recv();
            return reader->getRoot<messages::SensorQuery>();

        }

        void SensorBase::DispatchMessage(riaps::ports::PortBase* port) {
            component_logger()->debug("{}", __func__);
            component_logger()->error_if(port == nullptr, "port is null");
            auto portName = port->GetPortName();
            component_logger()->debug("Portname: {} macroname: {}", portName, PORT_TIMER_CLOCK);
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
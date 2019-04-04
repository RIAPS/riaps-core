//
// Created by istvan on 3/10/17.
//

#include <base/LocalEstimatorBase.h>
#include <componentmodel/r_pyconfigconverter.h>

using namespace std;
using namespace riaps::ports;

namespace distributedestimator {
    namespace components {

        LocalEstimatorBase::LocalEstimatorBase(const py::object *parent_actor,
                                               const py::dict actor_spec, // Actor json config
                                               const py::dict type_spec,  // component json config
                                               const std::string &name,
                                               const std::string &type_name,
                                               const py::dict args,
                                               const std::string &application_name,
                                               const std::string &actor_name)
                : ComponentBase(application_name, actor_name) {
            auto config = PyConfigConverter::convert(type_spec, actor_spec, args);
            config.component_name = name;
            config.component_type = type_name;
            config.is_device=false;
            set_config(config);
            set_debug_level(spd::level::info);
        }

        void LocalEstimatorBase::DispatchMessage(riaps::ports::PortBase* port) {
            auto port_name = port->port_name();
            if (port_name == PORT_SUB_READY) {
                OnReady();
            }
            if (port_name == PORT_REQ_QUERY) {
                OnQuery();
            }
        }

        riaps::ports::PortError LocalEstimatorBase::SendQuery(MessageBuilder<messages::SensorQuery>& message) {
            return SendMessageOnPort(message.capnp_builder(), PORT_REQ_QUERY);
        }

        std::tuple<MessageReader<messages::SensorValue>, riaps::ports::PortError> LocalEstimatorBase::RecvQuery() {
            auto port = GetRequestPortByName(PORT_REQ_QUERY);
            auto [msg_bytes, error] = port->Recv();
            MessageReader<messages::SensorValue> reader(msg_bytes);
            return make_tuple(reader, error);
        }

        std::tuple<MessageReader<messages::SensorReady>, riaps::ports::PortError> LocalEstimatorBase::RecvReady() {
            auto port = GetPortAs<riaps::ports::SubscriberPort>(PORT_SUB_READY);
            auto [msg_bytes, error] = port->Recv();
            MessageReader<messages::SensorReady> reader(msg_bytes);
            return make_tuple(reader, error);
        }

        void LocalEstimatorBase::DispatchInsideMessage(zmsg_t *zmsg, riaps::ports::PortBase *port) {

        }

        riaps::ports::PortError LocalEstimatorBase::SendEstimate(MessageBuilder<messages::Estimate>& message) {
            return SendMessageOnPort(message.capnp_builder(), PORT_PUB_ESTIMATE);
        }


    }
}
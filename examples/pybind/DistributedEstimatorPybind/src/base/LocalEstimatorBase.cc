//
// Created by istvan on 3/10/17.
//

#include <base/LocalEstimatorBase.h>
#include <componentmodel/r_pyconfigconverter.h>

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
            auto config = PyConfigConverter::convert(type_spec, actor_spec);
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

        bool LocalEstimatorBase::SendQuery(MessageBuilder<messages::SensorQuery>& message) {
            return SendMessageOnPort(message.capnp_builder(), PORT_REQ_QUERY);
        }

        messages::SensorValue::Reader LocalEstimatorBase::RecvQuery() {
            auto port = GetRequestPortByName(PORT_REQ_QUERY);
            auto reader = port->AsRecvPort()->Recv();
            return reader->getRoot<messages::SensorValue>();
//            capnp::FlatArrayMessageReader* messageReader;
//
//            if (port->Recv(&messageReader)){
//                message = messageReader->getRoot<messages::SensorValue>();
//                return true;
//            }
//
//            return false;
        }

        messages::SensorReady::Reader LocalEstimatorBase::RecvReady() {
            auto port = GetPortAs<riaps::ports::SubscriberPort>(PORT_SUB_READY);
            auto reader = port->Recv();
            auto r = reader->getRoot<messages::SensorReady>();
            return r;
        }

        void LocalEstimatorBase::DispatchInsideMessage(zmsg_t *zmsg, riaps::ports::PortBase *port) {

        }

        bool LocalEstimatorBase::SendEstimate(MessageBuilder<messages::Estimate>& message) {
            return SendMessageOnPort(message.capnp_builder(), PORT_PUB_ESTIMATE);
        }


    }
}
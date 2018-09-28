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
            config.isDevice=false;
            set_config(config);
        }

        void LocalEstimatorBase::DispatchMessage(capnp::FlatArrayMessageReader* capnpreader,
                                                 riaps::ports::PortBase *port,
                                                 std::shared_ptr<riaps::MessageParams> params) {
            if (port->GetPortName() == PORT_SUB_READY) {
                messages::SensorReady::Reader sensorReady = capnpreader->getRoot<messages::SensorReady>();
                OnReady(sensorReady, port);
            }
        }

        bool LocalEstimatorBase::SendQuery(capnp::MallocMessageBuilder &messageBuilder,
                                           messages::SensorQuery::Builder &message) {
            return SendMessageOnPort(messageBuilder, PORT_REQ_QUERY);
        }

        bool LocalEstimatorBase::RecvQuery(messages::SensorValue::Reader &message) {
            auto port = GetRequestPortByName(PORT_REQ_QUERY);
            if (port == NULL) return false;

            // TODO: with share_ptr it would be better, DSML update is also required
            capnp::FlatArrayMessageReader* messageReader;

            if (port->Recv(&messageReader)){
                message = messageReader->getRoot<messages::SensorValue>();
                return true;
            }

            return false;
        }

        void LocalEstimatorBase::DispatchInsideMessage(zmsg_t *zmsg, riaps::ports::PortBase *port) {

        }

        bool LocalEstimatorBase::SendEstimate(capnp::MallocMessageBuilder &messageBuilder,
                                              messages::Estimate::Builder &message) {
            return SendMessageOnPort(messageBuilder, PORT_PUB_ESTIMATE);
        }

        LocalEstimatorBase::~LocalEstimatorBase() {

        }
    }
}
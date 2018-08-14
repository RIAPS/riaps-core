//
// Created by istvan on 11/11/16.
//

#include "LocalEstimator.h"

namespace distributedestimator {
    namespace components {


        LocalEstimator::LocalEstimator(const py::object *parent_actor,
                                       const py::dict actor_spec, // Actor json config
                                       const py::dict type_spec,  // component json config
                                       const std::string &name,
                                       const std::string &type_name,
                                       const py::dict args,
                                       const std::string &application_name,
                                       const std::string &actor_name)
                : LocalEstimatorBase(parent_actor, actor_spec, type_spec, name, type_name, args, application_name,
                                      actor_name) {

        }

        void LocalEstimator::OnReady(const messages::SensorReady::Reader &message,
                                     riaps::ports::PortBase *port) {

            //PrintMessageOnPort(port, message.getMsg().cStr());

            _logger->info("LocalEstimator::OnReady(): {} {}", message.getMsg().cStr(), ::getpid());

            capnp::MallocMessageBuilder builderSensorQuery;

            messages::SensorQuery::Builder queryMsg = builderSensorQuery.initRoot<messages::SensorQuery>();

            queryMsg.setMsg("sensor_query");
            auto result = SendQuery(builderSensorQuery, queryMsg);
            if (result) {
                messages::SensorValue::Reader sensorValue;
                if (RecvQuery(sensorValue)) {
                    if (GetPortByName(PORT_REQ_QUERY)->GetPortBaseConfig()->isTimed) {
                        auto recvPort = GetPortByName(PORT_REQ_QUERY)->AsRecvPort();
                        if (recvPort!=nullptr){
                            _logger->info("LocalEstimator::OnQuery(): {};  sentTimestamp: {}.{}, recvTimestamp: {}.{}",
                                          sensorValue.getMsg().cStr(),
                                          recvPort->GetLastSendTimestamp().tv_sec ,
                                          recvPort->GetLastSendTimestamp().tv_nsec,
                                          recvPort->GetLastRecvTimestamp().tv_sec ,
                                          recvPort->GetLastRecvTimestamp().tv_nsec
                            );
                        }
                    } else {
                        _logger->info("LocalEstimator::OnQuery(): {}", sensorValue.getMsg().cStr());
                    }

                    capnp::MallocMessageBuilder builderEstimate;
                    auto estimateMsg = builderEstimate.initRoot<messages::Estimate>();
                    estimateMsg.setMsg("local_est(" + std::to_string(::getpid()) + ")");
                    SendEstimate(builderEstimate, estimateMsg);
                }
            }
        }



        LocalEstimator::~LocalEstimator() {

        }
    }
}

//riaps::ComponentBase *create_component(_component_conf &config, riaps::Actor &actor) {
//    auto result = new distributedestimator::components::LocalEstimator(config, actor);
//    return result;
//}
//
//void destroy_component(riaps::ComponentBase *comp) {
//    delete comp;
//}

std::unique_ptr<distributedestimator::components::LocalEstimator>
create_component_py(const py::object *parent_actor,
                    const py::dict actor_spec,
                    const py::dict type_spec,
                    const std::string &name,
                    const std::string &type_name,
                    const py::dict args,
                    const std::string &application_name,
                    const std::string &actor_name) {
    auto ptr = new distributedestimator::components::LocalEstimator(parent_actor, actor_spec, type_spec, name, type_name, args,
                                                                     application_name,
                                                                     actor_name);
    return std::move(std::unique_ptr<distributedestimator::components::LocalEstimator>(ptr));
}

PYBIND11_MODULE(localestimator, m) {
    py::class_<distributedestimator::components::LocalEstimator> testClass(m, "LocalEstimator");
    testClass.def(py::init<const py::object*, const py::dict, const py::dict, const std::string&, const std::string&, const py::dict, const std::string&, const std::string&>());
    testClass.def("setup", &distributedestimator::components::LocalEstimator::setup);
    testClass.def("activate", &distributedestimator::components::LocalEstimator::activate);
    testClass.def("handlePortUpdate", &distributedestimator::components::LocalEstimator::HandlePortUpdate);
    m.def("create_component_py", &create_component_py, "Instantiates the component from python configuration");
}
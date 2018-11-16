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

        void LocalEstimator::OnQuery() {
            auto msg = RecvQuery();
            component_logger()->info("{}: [{}]", msg.getMsg().cStr(), ::getpid());
        }

        void LocalEstimator::OnReady() {
            auto msg = RecvReady();
            component_logger()->info("{}: {} {}", __func__, msg.getMsg().cStr(), ::getpid());

            capnp::MallocMessageBuilder builderSensorQuery;
            messages::SensorQuery::Builder queryMsg = builderSensorQuery.initRoot<messages::SensorQuery>();
            queryMsg.setMsg("sensor_query");
            auto result = SendQuery(builderSensorQuery, queryMsg);
            if (result) {
                auto value = RecvQuery();
                capnp::MallocMessageBuilder builderEstimate;
                auto estimateMsg = builderEstimate.initRoot<messages::Estimate>();
                estimateMsg.setMsg(fmt::format("local_est({})", ::getpid()));
                SendEstimate(builderEstimate, estimateMsg);
//                messages::SensorValue::Reader sensorValue;
//                if (RecvQuery(sensorValue)) {
//                    if (GetPortByName(PORT_REQ_QUERY)->GetPortBaseConfig()->isTimed) {
//                        auto recvPort = GetPortByName(PORT_REQ_QUERY)->AsRecvPort();
//                        if (recvPort!=nullptr){
//                            component_logger()->info("LocalEstimator::OnQuery(): {};  sentTimestamp: {}.{}, recvTimestamp: {}.{}",
//                                          sensorValue.getMsg().cStr(),
//                                          recvPort->GetLastSendTimestamp().tv_sec ,
//                                          recvPort->GetLastSendTimestamp().tv_nsec,
//                                          recvPort->GetLastRecvTimestamp().tv_sec ,
//                                          recvPort->GetLastRecvTimestamp().tv_nsec
//                            );
//                        }
//                    } else {
//                        component_logger()->info("LocalEstimator::OnQuery(): {}", sensorValue.getMsg().cStr());
//                    }
//
//                    capnp::MallocMessageBuilder builderEstimate;
//                    auto estimateMsg = builderEstimate.initRoot<messages::Estimate>();
//                    estimateMsg.setMsg("local_est(" + std::to_string(::getpid()) + ")");
//                    SendEstimate(builderEstimate, estimateMsg);
//                }
            } else {
                component_logger()->error("Error sending query");
            }
        }

        LocalEstimator::~LocalEstimator() {

        }
    }
}

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

PYBIND11_MODULE(liblocalestimator, m) {
    py::class_<distributedestimator::components::LocalEstimator> testClass(m, "LocalEstimator");
    testClass.def(py::init<const py::object*, const py::dict, const py::dict, const std::string&, const std::string&, const py::dict, const std::string&, const std::string&>());

    testClass.def("setup"                 , &distributedestimator::components::LocalEstimator::Setup);
    testClass.def("activate"              , &distributedestimator::components::LocalEstimator::Activate);
    testClass.def("terminate"             , &distributedestimator::components::LocalEstimator::Terminate);
    testClass.def("handlePortUpdate"      , &distributedestimator::components::LocalEstimator::HandlePortUpdate);
    testClass.def("handleCPULimit"        , &distributedestimator::components::LocalEstimator::HandleCPULimit);
    testClass.def("handleMemLimit"        , &distributedestimator::components::LocalEstimator::HandleMemLimit);
    testClass.def("handleSpcLimit"        , &distributedestimator::components::LocalEstimator::HandleSpcLimit);
    testClass.def("handleNetLimit"        , &distributedestimator::components::LocalEstimator::HandleNetLimit);
    testClass.def("handleNICStateChange"  , &distributedestimator::components::LocalEstimator::HandleNICStateChange);
    testClass.def("handlePeerStateChange" , &distributedestimator::components::LocalEstimator::HandlePeerStateChange);

    m.def("create_component_py", &create_component_py, "Instantiates the component from python configuration");
}
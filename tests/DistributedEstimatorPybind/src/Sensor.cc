//
// Created by istvan on 11/11/16.
//

#include "Sensor.h"
#include <capnp/serialize.h>
#include <capnp/message.h>

namespace distributedestimator {
    namespace components {

        Sensor::Sensor(const py::object *parent_actor,
                       const py::dict actor_spec, // Actor json config
                       const py::dict type_spec,  // component json config
                       const std::string &name,
                       const std::string &type_name,
                       const py::dict args,
                       const std::string &application_name,
                       const std::string &actor_name)
                : SensorBase(parent_actor, actor_spec, type_spec, name, type_name, args, application_name,
                                      actor_name) {
        }

        void Sensor::OnClock(riaps::ports::PortBase *port) {
            int64_t time = zclock_mono();
            component_logger()->info("Sensor::OnClock(): {}", time);

            capnp::MallocMessageBuilder messageBuilder;
            auto msgSensorReady = messageBuilder.initRoot<messages::SensorReady>();
            msgSensorReady.setMsg("data_ready");

            SendReady(messageBuilder, msgSensorReady);
        }

        void Sensor::OnRequest(const messages::SensorQuery::Reader &message,
                                    riaps::ports::PortBase *port) {

            if (port->GetPortBaseConfig()->isTimed){
                auto responsePort = port->AsResponsePort();
                component_logger()->info_if(responsePort!=nullptr,
                                 "Sensor::OnRequest(): {}, sentTimestamp: {}.{}, recvTimestamp: {}.{}",
                                 message.getMsg().cStr(),
                                 responsePort->GetLastSendTimestamp().tv_sec ,
                                 responsePort->GetLastSendTimestamp().tv_nsec,
                                 responsePort->GetLastRecvTimestamp().tv_sec ,
                                 responsePort->GetLastRecvTimestamp().tv_nsec);
            } else
                component_logger()->info("Sensor::OnRequest(): {}", message.getMsg().cStr());

            capnp::MallocMessageBuilder messageBuilder;
            messages::SensorValue::Builder msgSensorValue = messageBuilder.initRoot<messages::SensorValue>();
            msgSensorValue.setMsg("sensor_rep");

            if (!SendRequest(messageBuilder, msgSensorValue)){
                // Couldn't send the response
            }
        }



        Sensor::~Sensor() {

        }
    }
}

std::unique_ptr<distributedestimator::components::Sensor>
create_component_py(const py::object *parent_actor,
                    const py::dict actor_spec,
                    const py::dict type_spec,
                    const std::string &name,
                    const std::string &type_name,
                    const py::dict args,
                    const std::string &application_name,
                    const std::string &actor_name) {
    auto ptr = new distributedestimator::components::Sensor(parent_actor, actor_spec, type_spec, name, type_name, args,
                                                                     application_name,
                                                                     actor_name);
    return std::move(std::unique_ptr<distributedestimator::components::Sensor>(ptr));
}

PYBIND11_MODULE(libsensor, m) {
    py::class_<distributedestimator::components::Sensor> testClass(m, "Sensor");
    testClass.def(py::init<const py::object*, const py::dict, const py::dict, const std::string&, const std::string&, const py::dict, const std::string&, const std::string&>());

    testClass.def("setup"                 , &distributedestimator::components::Sensor::Setup);
    testClass.def("activate"              , &distributedestimator::components::Sensor::Activate);
    testClass.def("terminate"             , &distributedestimator::components::Sensor::Terminate);
    testClass.def("handlePortUpdate"      , &distributedestimator::components::Sensor::HandlePortUpdate);
    testClass.def("handleCPULimit"        , &distributedestimator::components::Sensor::HandleCPULimit);
    testClass.def("handleMemLimit"        , &distributedestimator::components::Sensor::HandleMemLimit);
    testClass.def("handleSpcLimit"        , &distributedestimator::components::Sensor::HandleSpcLimit);
    testClass.def("handleNetLimit"        , &distributedestimator::components::Sensor::HandleNetLimit);
    testClass.def("handleNICStateChange"  , &distributedestimator::components::Sensor::HandleNICStateChange);
    testClass.def("handlePeerStateChange" , &distributedestimator::components::Sensor::HandlePeerStateChange);

    m.def("create_component_py", &create_component_py, "Instantiates the component from python configuration");
}
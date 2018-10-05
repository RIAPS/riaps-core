#include <TempSensor.h>

namespace weathermonitor {
    namespace components {		

        TempSensor::TempSensor(const py::object *parent_actor,
							   const py::dict actor_spec, // Actor json config
							   const py::dict type_spec,  // component json config
							   const std::string &name,
							   const std::string &type_name,
							   const py::dict args,
							   const std::string &application_name,
							   const std::string &actor_name) :
                TempSensorBase(parent_actor, actor_spec, type_spec, name, type_name, args, application_name,
							   actor_name) {
			
			this->temperature = 65;
        }

		void TempSensor::OnClock(riaps::ports::PortBase *port) {
        	component_logger()->info("{}", __func__);

			this->temperature += 1;
			capnp::MallocMessageBuilder messageBuilder;
			auto msgTempData = messageBuilder.initRoot<messages::TempData>();
			msgTempData.setTempature(this->temperature);
			
			if (!SendReady(messageBuilder, msgTempData)){
				component_logger()->error("{}: Unable to publish temperature!", __func__);
            }
		}

    }
}

std::unique_ptr<weathermonitor::components::TempSensor>
create_component_py(const py::object *parent_actor,
					const py::dict actor_spec,
					const py::dict type_spec,
					const std::string &name,
					const std::string &type_name,
					const py::dict args,
					const std::string &application_name,
					const std::string &actor_name) {
	auto ptr = new weathermonitor::components::TempSensor(parent_actor, actor_spec, type_spec, name, type_name, args,
																	 application_name,
																	 actor_name);
	return std::move(std::unique_ptr<weathermonitor::components::TempSensor>(ptr));
}

PYBIND11_MODULE(libtempsensor, m) {
py::class_<weathermonitor::components::TempSensor> testClass(m, "TempSensor");
testClass.def(py::init<const py::object*, const py::dict, const py::dict, const std::string&, const std::string&, const py::dict, const std::string&, const std::string&>());

testClass.def("setup"                 , &weathermonitor::components::TempSensor::Setup);
testClass.def("activate"              , &weathermonitor::components::TempSensor::Activate);
testClass.def("terminate"             , &weathermonitor::components::TempSensor::Terminate);
testClass.def("handlePortUpdate"      , &weathermonitor::components::TempSensor::HandlePortUpdate);
testClass.def("handleCPULimit"        , &weathermonitor::components::TempSensor::HandleCPULimit);
testClass.def("handleMemLimit"        , &weathermonitor::components::TempSensor::HandleMemLimit);
testClass.def("handleSpcLimit"        , &weathermonitor::components::TempSensor::HandleSpcLimit);
testClass.def("handleNetLimit"        , &weathermonitor::components::TempSensor::HandleNetLimit);
testClass.def("handleNICStateChange"  , &weathermonitor::components::TempSensor::HandleNICStateChange);
testClass.def("handlePeerStateChange" , &weathermonitor::components::TempSensor::HandlePeerStateChange);

m.def("create_component_py", &create_component_py, "Instantiates the component from python configuration");
}

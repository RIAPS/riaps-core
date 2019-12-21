



#include <componentmodel/r_pyconfigconverter.h>
#include <base/GroupieCppBase.h>

using namespace std;
using namespace riaps::ports;

namespace groupapp {
    namespace components {
        GroupieCppBase::GroupieCppBase(const py::object*  parent_actor     ,
                          const py::dict     actor_spec       ,
                          const py::dict     type_spec        ,
                          const std::string& name             ,
                          const std::string& type_name        ,
                          const py::dict     args             ,
                          const std::string& application_name ,
                          const std::string& actor_name       ,
                          const py::list     groups) : ComponentBase(application_name, actor_name){
            auto conf = PyConfigConverter::convert(type_spec, actor_spec, args);
            auto gr = PyConfigConverter::ConvertGroups(groups);
            conf.component_name = name;
            conf.component_type = type_name;
            conf.is_device = false;
            set_config(conf, gr);
            set_debug_level(spdlog::level::debug, spdlog::level::debug);
        }

        timespec GroupieCppBase::RecvClock() {
            auto port = GetPortAs<riaps::ports::PeriodicTimer>(PORT_TIMER_CLOCK);
            return port->Recv();
        }


        void GroupieCppBase::DispatchMessage(riaps::ports::PortBase* port) {
            auto port_name = port->port_name();
            if (port_name == PORT_TIMER_CLOCK) {
                OnClock();
            }
        }

        void GroupieCppBase::DispatchInsideMessage(zmsg_t *zmsg, riaps::ports::PortBase *port) { }
    }
}







#include <componentmodel/r_pyconfigconverter.h>
#include <base/TimerCompBase.h>

using namespace std;
using namespace riaps::ports;

namespace timersync {
    namespace components {
        TimerCompBase::TimerCompBase(const py::object*  parent_actor     ,
                          const py::dict     actor_spec       ,
                          const py::dict     type_spec        ,
                          const std::string& name             ,
                          const std::string& type_name        ,
                          const py::dict     args             ,
                          const std::string& application_name ,
                          const std::string& actor_name       ) : ComponentBase(application_name, actor_name){
            auto conf = PyConfigConverter::convert(type_spec, actor_spec);
            conf.component_name = name;
            conf.component_type = type_name;
            conf.is_device=false;
            set_config(conf);
        }

        timespec TimerCompBase::RecvPeriodic() {
            auto port = GetPortAs<riaps::ports::PeriodicTimer>(PORT_TIMER_PERIODIC);
            return port->Recv();
        }


        void TimerCompBase::DispatchMessage(riaps::ports::PortBase* port) {
            auto port_name = port->port_name();
            if (port_name == PORT_TIMER_PERIODIC) {
                OnPeriodic();
            }
        }

        void TimerCompBase::DispatchInsideMessage(zmsg_t *zmsg, riaps::ports::PortBase *port) { }
    }
}



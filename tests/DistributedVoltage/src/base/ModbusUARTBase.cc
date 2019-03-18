



#include <componentmodel/r_pyconfigconverter.h>
#include <base/ModbusUARTBase.h>

using namespace std;
using namespace riaps::ports;

namespace distributedvoltage {
    namespace components {
        ModbusUARTBase::ModbusUARTBase(const py::object*  parent_actor     ,
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

        timespec ModbusUARTBase::RecvClock() {
            auto port = GetPortAs<riaps::ports::PeriodicTimer>(PORT_TIMER_CLOCK);
            return port->Recv();
        }

        riaps::ports::PortError ModbusUARTBase::SendCurrentvoltage(MessageBuilder<messages::Voltage>& message) {
            return SendMessageOnPort(message.capnp_builder(), PORT_PUB_CURRENTVOLTAGE);
        }


        void ModbusUARTBase::DispatchMessage(riaps::ports::PortBase* port) {
            auto port_name = port->port_name();
            if (port_name == PORT_TIMER_CLOCK) {
                OnClock();
            }
        }

        void ModbusUARTBase::DispatchInsideMessage(zmsg_t *zmsg, riaps::ports::PortBase *port) { }
    }
}



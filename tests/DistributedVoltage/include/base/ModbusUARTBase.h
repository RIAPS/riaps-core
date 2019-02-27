

#ifndef MODBUSUARTBASE_H
#define MODBUSUARTBASE_H

#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <componentmodel/r_componentbase.h>
#include <componentmodel/r_messagebuilder.h>
#include <componentmodel/r_messagereader.h>
#include <messages/distributedvoltage.capnp.h>

namespace py = pybind11;
constexpr auto PORT_PUB_CURRENTVOLTAGE = "currentvoltage";
constexpr auto PORT_TIMER_CLOCK = "clock";


namespace distributedvoltage {
    namespace components {
        class ModbusUARTBase : public riaps::ComponentBase {
        public:
            ModbusUARTBase(const py::object*  parent_actor     ,
                          const py::dict     actor_spec       ,
                          const py::dict     type_spec        ,
                          const std::string& name             ,
                          const std::string& type_name        ,
                          const py::dict     args             ,
                          const std::string& application_name ,
                          const std::string& actor_name       );

            virtual void OnClock()=0;

            virtual timespec RecvClock() final;

            virtual riaps::ports::PortError SendCurrentvoltage(MessageBuilder<messages::Voltage>& message) final;

            virtual ~ModbusUARTBase() = default;
        protected:
            virtual void DispatchMessage(riaps::ports::PortBase* port) final;

            virtual void DispatchInsideMessage(zmsg_t* zmsg, riaps::ports::PortBase* port) final;
        };
    }
}


#endif // MODBUSUARTBASE_H

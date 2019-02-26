

#ifndef TIMERCOMPBASE_H
#define TIMERCOMPBASE_H

#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <componentmodel/r_componentbase.h>
#include <componentmodel/r_messagebuilder.h>
#include <componentmodel/r_messagereader.h>
#include <messages/timersync.capnp.h>

namespace py = pybind11;
constexpr auto PORT_TIMER_PERIODIC = "periodic";


namespace timersync {
    namespace components {
        class TimerCompBase : public riaps::ComponentBase {
        public:
            TimerCompBase(const py::object*  parent_actor     ,
                          const py::dict     actor_spec       ,
                          const py::dict     type_spec        ,
                          const std::string& name             ,
                          const std::string& type_name        ,
                          const py::dict     args             ,
                          const std::string& application_name ,
                          const std::string& actor_name       );

            virtual void OnPeriodic()=0;

            virtual timespec RecvPeriodic() final;


            virtual ~TimerCompBase() = default;
        protected:
            virtual void DispatchMessage(riaps::ports::PortBase* port) final;

            virtual void DispatchInsideMessage(zmsg_t* zmsg, riaps::ports::PortBase* port) final;
        };
    }
}


#endif // TIMERCOMPBASE_H

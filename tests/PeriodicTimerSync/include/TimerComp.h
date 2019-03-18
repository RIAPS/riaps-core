

#ifndef TIMERCOMP_H
#define TIMERCOMP_H
#include <base/TimerCompBase.h>
// riaps:keep_header:begin

// riaps:keep_header:end>>

namespace timersync {
    namespace components {
        class TimerComp : public TimerCompBase {
        public:
            TimerComp(const py::object*  parent_actor     ,
                          const py::dict     actor_spec       ,
                          const py::dict     type_spec        ,
                          const std::string& name             ,
                          const std::string& type_name        ,
                          const py::dict     args             ,
                          const std::string& application_name ,
                          const std::string& actor_name       );


            virtual void OnPeriodic() override;

            virtual ~TimerComp();

            // riaps:keep_decl:begin
        private:
            enum SyncStates {
                INIT,
                SYNC,
                RUNNING
            };

            SyncStates current_state_;

            // riaps:keep_decl:end
        };
    }
}

std::unique_ptr<timersync::components::TimerComp>
create_component_py(const py::object *parent_actor,
                    const py::dict actor_spec,
                    const py::dict type_spec,
                    const std::string &name,
                    const std::string &type_name,
                    const py::dict args,
                    const std::string &application_name,
const std::string &actor_name);

#endif // TIMERCOMP_H

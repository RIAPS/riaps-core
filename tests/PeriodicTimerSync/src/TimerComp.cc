


#include <TimerComp.h>
// riaps:keep_header:begin

// riaps:keep_header:end

namespace timersync {
    namespace components {

        // riaps:keep_construct:begin
        TimerComp::TimerComp(const py::object*  parent_actor     ,
                      const py::dict     actor_spec       ,
                      const py::dict     type_spec        ,
                      const std::string& name             ,
                      const std::string& type_name        ,
                      const py::dict     args             ,
                      const std::string& application_name ,
                      const std::string& actor_name       )
            : TimerCompBase(parent_actor, actor_spec, type_spec, name, type_name, args, application_name, actor_name) {
            current_state_ = TimerComp::SyncStates::INIT;
            component_logger()->set_pattern("%v");
            sched_param x{85};
            //x.__sched_priority = 85;
            auto ret = sched_setscheduler(0, SCHED_FIFO, &x);
            if (ret!=0)
                component_logger()->warn("Failed to set scheduler");
        }
        // riaps:keep_construct:end

        void TimerComp::OnPeriodic() {
            // riaps:keep_onperiodic:begin
            auto scheduled = RecvPeriodic();
            timespec current_time{0,0};
            clock_gettime(CLOCK_REALTIME, &current_time);

            if (current_state_ == SyncStates::INIT) {
                timespec wake_up = {
                            ((current_time.tv_sec + 2) / 10 + 1) * 10,
                            0
                        };
                auto timer_port = GetPortAs<riaps::ports::PeriodicTimer>(PORT_TIMER_PERIODIC);
                timer_port->Halt();
                //clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &wake_up, NULL);
                while (wake_up.tv_sec>current_time.tv_sec) {
                    clock_gettime(CLOCK_REALTIME, &current_time);
                }
                timer_port->Start();
                current_state_ = SyncStates::SYNC;
            } else {
                component_logger()->info("S\t{}\t{}", scheduled.tv_sec, scheduled.tv_nsec);
                component_logger()->info("F\t{}\t{}", current_time.tv_sec, current_time.tv_nsec);
            }

            // riaps:keep_onperiodic:end
        }

        // riaps:keep_impl:begin

        // riaps:keep_impl:end

        // riaps:keep_destruct:begin
        TimerComp::~TimerComp() {

        }
        // riaps:keep_destruct:end

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
                    const std::string &actor_name) {
    auto ptr = new timersync::components::TimerComp(parent_actor, actor_spec, type_spec, name, type_name, args,
                                                                     application_name,
                                                                     actor_name);
    return std::move(std::unique_ptr<timersync::components::TimerComp>(ptr));
}

PYBIND11_MODULE(libtimercomp, m) {
    py::class_<timersync::components::TimerComp> testClass(m, "TimerComp");
    testClass.def(py::init<const py::object*, const py::dict, const py::dict, const std::string&, const std::string&, const py::dict, const std::string&, const std::string&>());

    testClass.def("setup"                 , &timersync::components::TimerComp::Setup);
    testClass.def("activate"              , &timersync::components::TimerComp::Activate);
    testClass.def("terminate"             , &timersync::components::TimerComp::Terminate);
    testClass.def("handlePortUpdate"      , &timersync::components::TimerComp::HandlePortUpdate);
    testClass.def("handleCPULimit"        , &timersync::components::TimerComp::HandleCPULimit);
    testClass.def("handleMemLimit"        , &timersync::components::TimerComp::HandleMemLimit);
    testClass.def("handleSpcLimit"        , &timersync::components::TimerComp::HandleSpcLimit);
    testClass.def("handleNetLimit"        , &timersync::components::TimerComp::HandleNetLimit);
    testClass.def("handleNICStateChange"  , &timersync::components::TimerComp::HandleNICStateChange);
    testClass.def("handlePeerStateChange" , &timersync::components::TimerComp::HandlePeerStateChange);
    testClass.def("handleReinstate"       , &timersync::components::TimerComp::HandleReinstate);

    m.def("create_component_py", &create_component_py, "Instantiates the component from python configuration");
}


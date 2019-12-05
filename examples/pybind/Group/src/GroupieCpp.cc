


#include <GroupieCpp.h>
// riaps:keep_header:begin

// riaps:keep_header:end

namespace groupapp {
    namespace components {

        // riaps:keep_construct:begin
        GroupieCpp::GroupieCpp(const py::object*  parent_actor     ,
                      const py::dict     actor_spec       ,
                      const py::dict     type_spec        ,
                      const std::string& name             ,
                      const std::string& type_name        ,
                      const py::dict     args             ,
                      const std::string& application_name ,
                      const std::string& actor_name       ,
                      const py::list     groups)
            : GroupieCppBase(parent_actor, actor_spec, type_spec, name, type_name, args, application_name, actor_name, groups) {
        }
        // riaps:keep_construct:end

        void GroupieCpp::OnClock() {
            // riaps:keep_onclock:begin
            auto msg = RecvClock();
            component_logger()->info("{}", __FUNCTION__);
            MessageBuilder<messages::Msg> builder;
            builder->setValue("Kukucs");
            riaps::groups::GroupId id {"TheGroup", "g_1"};
            this->getGroupById(id)->Send(builder);
            // riaps:keep_onclock:end
        }

        // riaps:keep_impl:begin
        void GroupieCpp::HandleGroupMessage(riaps::groups::Group *group) {
            component_logger()->info("{}", __FUNCTION__);
            auto [msg, error] = group->Recv<messages::Msg>();
            if (!error)
                component_logger()->info(msg->getValue().cStr());
            else
                component_logger()->error(
                        "Couldn't read groupmessage in group {}::{}",
                        group->group_id().group_type_id,
                        group->group_id().group_name);
        }

        void GroupieCpp::HandleActivate() {
            if (JoinGroup({"TheGroup", "g_1"}))
                component_logger()->info("Joined g_1");
            else
                component_logger()->error("Couldn't join g_1");
        }
        // riaps:keep_impl:end

        // riaps:keep_destruct:begin
        GroupieCpp::~GroupieCpp() {

        }
        // riaps:keep_destruct:end

    }
}

std::unique_ptr<groupapp::components::GroupieCpp>
create_component_py(const py::object *parent_actor,
                    const py::dict actor_spec,
                    const py::dict type_spec,
                    const std::string &name,
                    const std::string &type_name,
                    const py::dict args,
                    const std::string &application_name,
                    const std::string &actor_name,
                    const py::list     groups) {
    auto ptr = new groupapp::components::GroupieCpp(parent_actor, actor_spec, type_spec, name, type_name, args,
                                                                     application_name,
                                                                     actor_name, groups);
    return std::move(std::unique_ptr<groupapp::components::GroupieCpp>(ptr));
}

PYBIND11_MODULE(libgroupiecpp, m) {
    py::class_<groupapp::components::GroupieCpp> testClass(m, "GroupieCpp");
    testClass.def(py::init<const py::object*, const py::dict, const py::dict, const std::string&, const std::string&, const py::dict, const std::string&, const std::string&, const py::list>());

    testClass.def("setup"                 , &groupapp::components::GroupieCpp::Setup);
    testClass.def("activate"              , &groupapp::components::GroupieCpp::Activate);
    testClass.def("terminate"             , &groupapp::components::GroupieCpp::Terminate);
    testClass.def("handlePortUpdate"      , &groupapp::components::GroupieCpp::HandlePortUpdate);
    testClass.def("handleCPULimit"        , &groupapp::components::GroupieCpp::HandleCPULimit);
    testClass.def("handleMemLimit"        , &groupapp::components::GroupieCpp::HandleMemLimit);
    testClass.def("handleSpcLimit"        , &groupapp::components::GroupieCpp::HandleSpcLimit);
    testClass.def("handleNetLimit"        , &groupapp::components::GroupieCpp::HandleNetLimit);
    testClass.def("handleNICStateChange"  , &groupapp::components::GroupieCpp::HandleNICStateChange);
    testClass.def("handlePeerStateChange" , &groupapp::components::GroupieCpp::HandlePeerStateChange);
    testClass.def("handleReinstate"       , &groupapp::components::GroupieCpp::HandleReinstate);

    m.def("create_component_py", &create_component_py, "Instantiates the component from python configuration");
}


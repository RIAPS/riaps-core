

#ifndef GROUPIECPP_H
#define GROUPIECPP_H
#include <base/GroupieCppBase.h>
// riaps:keep_header:begin

// riaps:keep_header:end>>

namespace groupapp {
    namespace components {
        class GroupieCpp : public GroupieCppBase {
        public:
            GroupieCpp(const py::object*  parent_actor     ,
                          const py::dict     actor_spec       ,
                          const py::dict     type_spec        ,
                          const std::string& name             ,
                          const std::string& type_name        ,
                          const py::dict     args             ,
                          const std::string& application_name ,
                          const std::string& actor_name       ,
                          const py::list     groups);


            virtual void OnClock() override;

            virtual ~GroupieCpp();

            // riaps:keep_decl:begin

            // riaps:keep_decl:end
        };
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
                    const py::list     groups);

#endif // GROUPIECPP_H

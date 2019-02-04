#ifndef RIAPS_CORE_CONVERTER_H
#define RIAPS_CORE_CONVERTER_H

#include <componentmodel/r_configuration.h>
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <const/r_jsonmodel.h>

namespace py = pybind11;

class PyConfigConverter {
public:
    static ComponentConf convert(const py::dict& py_comp_config, const py::dict& py_actor) {
        ComponentConf result;
        result.is_device = false;
        ParseLocals(py_actor);
        auto json_ports  = py_comp_config[J_PORTS];
        auto json_pubs  = json_ports[J_PORTS_PUBS].cast<py::dict>();
        auto json_subs  = json_ports[J_PORTS_SUBS].cast<py::dict>();
        auto json_reqs  = json_ports[J_PORTS_REQS].cast<py::dict>();
        auto json_reps  = json_ports[J_PORTS_REPS].cast<py::dict>();
        auto json_tims  = json_ports[J_PORTS_TIMS].cast<py::dict>();

        PyConfigConverter::parse_pub_sub<ComponentPortSub>(json_subs, result.component_ports.subs);
        PyConfigConverter::parse_pub_sub<ComponentPortPub>(json_pubs, result.component_ports.pubs);
        PyConfigConverter::parse_req_rep<ComponentPortReq>(json_reqs, result.component_ports.reqs);
        PyConfigConverter::parse_req_rep<ComponentPortRep>(json_reps, result.component_ports.reps);
        PyConfigConverter::parse_tim(json_tims, result.component_ports.tims);

        return result;
    }

private:

    static std::set<std::string> locals_;

    static void ParseLocals(const py::dict& py_actor) {
        auto json_locals = py_actor[J_LOCALS];
        for (auto it = json_locals.begin(); it!=json_locals.end(); it++){
            locals_.insert(it->cast<py::dict>()["type"].cast<std::string>());
        }
    }

    static bool IsLocal(const std::string& message_type) {
        if (locals_.find(message_type) == locals_.end()) {
            return false;
        }
        return true;
    }

    // Todo: merge pub_sub and tim by enable_if
    template<class T>
    static void parse_pub_sub(const py::dict& ports, std::vector<T>& conf_container) {
        for (auto it = ports.begin(); it!=ports.end(); it++){
            T port;

            port.port_name    = it->first.cast<std::string>();
            port.message_type = it->second.cast<py::dict>()[J_TYPE].cast<std::string>();
            port.is_local     = IsLocal(port.message_type);
            conf_container.push_back(port);
        }
    };

    template<class T>
    static void parse_req_rep(const py::dict& ports, std::vector<T>& conf_container) {
        for (auto it = ports.begin(); it!=ports.end(); it++){
            T port;

            port.req_type     = it->second[J_PORT_REQTYPE].cast<std::string>();
            port.rep_type     = it->second[J_PORT_REPTYPE].cast<std::string>();
            port.is_local     = IsLocal(port.req_type) && IsLocal(port.rep_type);
            port.port_name    = it->first.cast<std::string>();
            port.message_type = port.req_type + "#" + port.rep_type;
            conf_container.push_back(port);
        }
    };

    static void parse_tim(const py::dict &ports,
                                      std::vector<ComponentPortTim>& conf_container) {
        for (auto it = ports.begin(); it!=ports.end(); it++){
            ComponentPortTim port;

            port.period       = it->second[J_PORT_PERIOD].cast<ulong>();
            port.port_name    = it->first.cast<std::string>();
            conf_container.push_back(port);
        }
    }
};

std::set<std::string> PyConfigConverter::locals_ = {};

#endif //RIAPS_CORE_CONVERTER_H

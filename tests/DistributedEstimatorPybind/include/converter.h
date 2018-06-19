//
// Created by istvan on 6/19/18.
//

#ifndef RIAPS_CORE_CONVERTER_H
#define RIAPS_CORE_CONVERTER_H

#include <componentmodel/r_configuration.h>
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <const/r_jsonmodel.h>

namespace py = pybind11;

class PyConfigConverter {
public:
    static component_conf convert(const py::dict& dict);

private:

    template<class T>
    static void parse_pub_sub(const py::dict& ports, std::vector<T>& conf_container) {
        for (auto it = ports.begin(); it!=ports.end(); it++){
            T port;
            port.portName    = it->first.cast<std::string>();
            port.messageType = it->second.cast<py::dict>()[J_TYPE].cast<std::string>();
            conf_container.push_back(port);
        }
    };

    template<class T>
    static void parse_req_rep(const py::dict& ports, std::vector<T>& conf_container) {
        for (auto it = ports.begin(); it!=ports.end(); it++){
            T port;

            port.req_type = it->second[J_PORT_REQTYPE].cast<std::string>();
            port.rep_type = it->second[J_PORT_REPTYPE].cast<std::string>();
            port.portName    = it->first.cast<std::string>();
            port.messageType = port.req_type + "#" + port.rep_type;
            conf_container.push_back(port);
        }
    };

    static void parse_tim(const py::dict& ports, std::vector<component_port_tim>& conf_container);
};

void c(py::dict& dict);

#endif //RIAPS_CORE_CONVERTER_H

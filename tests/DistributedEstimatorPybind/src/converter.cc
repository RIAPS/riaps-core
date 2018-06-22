//
// Created by istvan on 6/19/18.
//

#include <converter.h>
#include <spdlog/spdlog.h>


namespace spd = spdlog;

using namespace std;

component_conf PyConfigConverter::convert(const py::dict& dict) {
    component_conf result;
    result.isDevice = false;

    auto json_ports = dict[J_PORTS];
    auto json_pubs  = json_ports[J_PORTS_PUBS].cast<py::dict>();
    auto json_subs  = json_ports[J_PORTS_SUBS].cast<py::dict>();
    auto json_reqs  = json_ports[J_PORTS_REQS].cast<py::dict>();
    auto json_reps  = json_ports[J_PORTS_REPS].cast<py::dict>();
    auto json_tims  = json_ports[J_PORTS_TIMS].cast<py::dict>();

    PyConfigConverter::parse_pub_sub<component_port_sub>(json_subs, result.component_ports.subs);
    PyConfigConverter::parse_pub_sub<component_port_pub>(json_pubs, result.component_ports.pubs);
    PyConfigConverter::parse_req_rep<component_port_req>(json_reqs, result.component_ports.reqs);
    PyConfigConverter::parse_req_rep<component_port_rep>(json_reps, result.component_ports.reps);
    PyConfigConverter::parse_tim(json_tims, result.component_ports.tims);

    return result;
}

void PyConfigConverter::parse_tim(const py::dict &ports,
                                  std::vector<component_port_tim>& conf_container) {
    for (auto it = ports.begin(); it!=ports.end(); it++){
        component_port_tim port;

        port.period = it->second[J_PORT_PERIOD].cast<ulong>();
        port.portName    = it->first.cast<std::string>();
        conf_container.push_back(port);
    }
}

//std::map<PyConfigConverter::PORT_TYPES, std::string> PyConfigConverter::port_type_mapping_ = {
//{PyConfigConverter::PORT_TYPES::PUB, "pubs"},
//{PyConfigConverter::PORT_TYPES::SUB, "subs"},
//};



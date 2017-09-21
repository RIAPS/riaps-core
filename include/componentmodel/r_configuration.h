//
// Created by parallels on 9/7/16.
//

#ifndef RIAPS_FW_R_CONFIGURATION_H
#define RIAPS_FW_R_CONFIGURATION_H

#include <componentmodel/r_parameter.h>

#include <string>
#include <vector>

using namespace riaps::componentmodel;

//// New part

struct _component_port_config {
    std::string portName;
    bool        isLocal;
    std::string messageType;

    _component_port_config(){
        isLocal=false;
    }
};

typedef struct _component_port_config component_port_config;

struct _component_port_clt : public component_port_config {};
struct _component_port_pub : public component_port_config {};

struct _component_port_req : public component_port_config {
    std::string req_type;
    std::string rep_type;
};

struct _component_port_rep : public component_port_config{
    std::string req_type;
    std::string rep_type;
};
struct _component_port_srv : public component_port_config{};
struct _component_port_sub : public component_port_config{
    std::string messageType;
};
struct _component_port_tim : public component_port_config {
    ulong       period;
};
struct _component_port_ins : public component_port_config{

};



struct _component_ports {
    std::vector<_component_port_clt> clts;
    std::vector<_component_port_srv> srvs;
    std::vector<_component_port_pub> pubs;
    std::vector<_component_port_sub> subs;
    std::vector<_component_port_req> reqs;
    std::vector<_component_port_rep> reps;
    std::vector<_component_port_tim> tims;
    std::vector<_component_port_ins> inss;
};


struct _component_conf {
    std::string        component_name;
    std::string        component_type;
    _component_ports   component_ports;
    Parameters         component_parameters;
    bool               isDevice;
};

typedef struct _component_conf component_conf;

#endif //RIAPS_FW_R_CONFIGURATION_H

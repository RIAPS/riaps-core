//
// Created by parallels on 9/7/16.
//

#ifndef RIAPS_FW_R_CONFIGURATION_H
#define RIAPS_FW_R_CONFIGURATION_H

#include <componentmodel/r_parameter.h>

#include <string>
#include <vector>

using namespace riaps::componentmodel;

/*
struct _localport_conf{
    std::string servicename;   // Name of the service, this will be registered in the discovery service.
    int         port;          // If 0, the po assigne automatically
    std::string network_iface; // e.g.: eth0 and not the IP address
};

typedef struct _localport_conf publisher_conf;
typedef struct _localport_conf response_conf;

struct _remoteport_conf {
    std::string servicename;        // Name of the service, this will be registered in the discovery service.
    std::string remoteservice_name; // Remote service, where this port will connect to.
};

typedef struct _remoteport_conf subscriber_conf;
typedef struct _remoteport_conf request_conf;

struct _periodic_timer_conf{
    std::string timerid;  // Must be unique (component scope)
    int         interval; // in millisec
};

typedef struct _periodic_timer_conf periodic_timer_conf;

struct _component_conf {
    std::string                      component_name;        // name of the component, it will be registered in the disc. serv.
    std::vector<publisher_conf>      publishers_config;     // publisher ports
    std::vector<subscriber_conf>     subscribers_config;    // subscriber ports
    std::vector<response_conf>       responses_config;      // server ports
    std::vector<request_conf>        requests_config;       // client ports
    std::vector<periodic_timer_conf> periodic_timer_config; // periodic timers
};

typedef struct _component_conf component_conf;
*/

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

struct _component_port_clt_j : public component_port_config {};
struct _component_port_pub_j : public component_port_config {};

struct _component_port_req_j : public component_port_config {
    std::string req_type;
    std::string rep_type;
};

struct _component_port_rep_j : public component_port_config{
    std::string req_type;
    std::string rep_type;
};
struct _component_port_srv_j : public component_port_config{};
struct _component_port_sub_j : public component_port_config{
    std::string messageType;
};
struct _component_port_tim_j : public component_port_config {
    ulong       period;
};
struct _component_port_ins_j : public component_port_config{

};



struct _component_ports_j {
    std::vector<_component_port_clt_j> clts;
    std::vector<_component_port_srv_j> srvs;
    std::vector<_component_port_pub_j> pubs;
    std::vector<_component_port_sub_j> subs;
    std::vector<_component_port_req_j> reqs;
    std::vector<_component_port_rep_j> reps;
    std::vector<_component_port_tim_j> tims;
    std::vector<_component_port_ins_j> inss;
};


// TODO: Refactor names
struct _component_conf_j {
    std::string        component_name;
    std::string        component_type;
    _component_ports_j component_ports;
    Parameters         component_parameters;
    bool               isDevice;
};

typedef struct _component_conf_j component_conf_j;

#endif //RIAPS_FW_R_CONFIGURATION_H

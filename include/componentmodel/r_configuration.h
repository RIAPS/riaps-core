//
// Created by parallels on 9/7/16.
//

#ifndef RIAPS_FW_R_CONFIGURATION_H
#define RIAPS_FW_R_CONFIGURATION_H

#include <componentmodel/r_parameter.h>

#include <string>
#include <vector>

using namespace riaps::componentmodel;

/**
 * Baseclass for component and group ports. Contains portName - messageType mapping.
 */
struct PortConfigBase {
    std::string port_name;
    std::string message_type;
};

/**
 * Port-message type mapping for groups
 */
struct GroupPortConfig : public PortConfigBase{
};

/**
 * Port-message type mapping for groups
 * Component port can be local.
 */
struct ComponentPortConfig : public PortConfigBase {
    bool        is_local;
    bool        is_timed;

    ComponentPortConfig(){
        is_local=false;
        is_timed=false;
    }
};


struct ComponentPortClt : public ComponentPortConfig {};
struct ComponentPortPub : public ComponentPortConfig {};

struct ComponentPortReq : public ComponentPortConfig {
    std::string req_type;
    std::string rep_type;
};

struct ComponentPortRep : public ComponentPortConfig{
    std::string req_type;
    std::string rep_type;
};
struct ComponentPortSrv : public ComponentPortConfig{};
struct ComponentPortSub : public ComponentPortConfig{
};
struct ComponentPortTim : public ComponentPortConfig {
    ulong period;
};
struct ComponentPortIns : public ComponentPortConfig{

};

struct ComponentPortQry : public ComponentPortConfig {
    std::string qry_type;
    std::string ans_type;
};

struct ComponentPortAns : public ComponentPortConfig{
    std::string qry_type;
    std::string ans_type;
};



struct ComponentPorts {
    std::vector<ComponentPortClt> clts; // Client
    std::vector<ComponentPortSrv> srvs; // Server
    std::vector<ComponentPortPub> pubs; // Publisher
    std::vector<ComponentPortSub> subs; // Subscribe
    std::vector<ComponentPortReq> reqs; // Request
    std::vector<ComponentPortRep> reps; // Response
    std::vector<ComponentPortTim> tims; // Timer
    std::vector<ComponentPortIns> inss; // Inside
    std::vector<ComponentPortQry> qrys; // Query
    std::vector<ComponentPortAns> anss; // Answer
};

/**
 * List of ports for group types
 * Only pub/sub
 */
typedef struct ComponentPortPub GroupPortPub;
typedef struct ComponentPortSub GroupPortSub;
struct groupt_ports {
    std::vector<GroupPortPub> pubs;
    std::vector<GroupPortSub> subs;
};

/**
 * Grouptype configuration
 */
struct GroupTypeConf {
    std::string   group_type_id;
    groupt_ports  group_type_ports;
    bool          has_leader;
    bool          has_consensus;
};


struct ComponentConf {
    std::string               component_name;
    std::string               component_type;
    ComponentPorts            component_ports;
    Parameters                component_parameters;
    bool                      is_device;
};

#endif //RIAPS_FW_R_CONFIGURATION_H

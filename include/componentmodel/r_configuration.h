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
struct port_config_base {
    std::string portName;
    std::string messageType;
};

/**
 * Port-message type mapping for groups
 */
struct group_port_config : public port_config_base{
};

/**
 * Port-message type mapping for groups
 * Component port can be local.
 */
struct component_port_config : public port_config_base {
    bool        isLocal;
    bool        isTimed;

    component_port_config(){
        isLocal=false;
        isTimed=false;
    }
};


struct component_port_clt : public component_port_config {};
struct component_port_pub : public component_port_config {};

struct component_port_req : public component_port_config {
//    component_port_req(){
//        isTimed = true;
//    }

    std::string req_type;
    std::string rep_type;
};

struct component_port_rep : public component_port_config{

//    //DEBUG todo: remove
//    component_port_rep(){
//        isTimed = true;
//    }

    std::string req_type;
    std::string rep_type;
};
struct component_port_srv : public component_port_config{};
struct component_port_sub : public component_port_config{
    //std::string messageType;
};
struct component_port_tim : public component_port_config {
    ulong       period;
};
struct component_port_ins : public component_port_config{

};

struct component_port_qry : public component_port_config {
    std::string qry_type;
    std::string ans_type;
};

struct component_port_ans : public component_port_config{
    std::string qry_type;
    std::string ans_type;
};



struct ComponentPorts {
    std::vector<component_port_clt> clts; // Client
    std::vector<component_port_srv> srvs; // Server
    std::vector<component_port_pub> pubs; // Publisher
    std::vector<component_port_sub> subs; // Subscribe
    std::vector<component_port_req> reqs; // Request
    std::vector<component_port_rep> reps; // Response
    std::vector<component_port_tim> tims; // Timer
    std::vector<component_port_ins> inss; // Inside
    std::vector<component_port_qry> qrys; // Query
    std::vector<component_port_ans> anss; // Answer
};

/**
 * List of ports for group types
 * Only pub/sub
 */
typedef struct component_port_pub group_port_pub;
typedef struct component_port_sub group_port_sub;
struct groupt_ports {
    std::vector<group_port_pub> pubs;
    std::vector<group_port_sub> subs;
};

/**
 * Grouptype configuration
 */
struct groupt_conf {
    std::string   groupTypeId;
    groupt_ports  groupTypePorts;
    bool          hasLeader;
    bool          hasConsensus;
};


struct component_conf {
    std::string               component_name;
    std::string               component_type;
    ComponentPorts            component_ports;
    Parameters                component_parameters;
    bool                      isDevice;
};

typedef struct component_conf _component_conf;

#endif //RIAPS_FW_R_CONFIGURATION_H

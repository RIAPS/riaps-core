//
// Created by istvan on 2/21/17.
//

#ifndef RIAPS_CORE_R_JSONMODEL_H_H
#define RIAPS_CORE_R_JSONMODEL_H_H

constexpr auto J_NAME         = "name";
constexpr auto J_ACTORS       = "actors";
constexpr auto J_COMPONENTS   = "components";
constexpr auto J_DEVICES      = "devices";

constexpr auto J_INSTANCES    = "instances";
constexpr auto J_INTERNALS    = "internals";
constexpr auto J_LOCALS       = "locals";

constexpr auto J_FORMALS      = "formals";
constexpr auto J_FORMAL_NAME  = "name";
constexpr auto J_FORMAL_DEF   = "default";

constexpr auto J_ACTUALS      = "actuals";
constexpr auto J_ACTUAL_NAME  = "name";
constexpr auto J_ACTUAL_PARAM = "param";
constexpr auto J_ACTUAL_VALUE = "value";

constexpr auto J_PORTS        = "ports";
constexpr auto J_PORTS_CLTS   = "clts";
constexpr auto J_PORTS_INSS   = "inss";
constexpr auto J_PORTS_PUBS   = "pubs";
constexpr auto J_PORTS_REPS   = "reps";
constexpr auto J_PORTS_REQS   = "reqs";
constexpr auto J_PORTS_SRVS   = "srvs";
constexpr auto J_PORTS_SUBS   = "subs";
constexpr auto J_PORTS_TIMS   = "tims";
constexpr auto J_PORTS_QRYS   = "qrys";
constexpr auto J_PORTS_ANSS   = "anss";

constexpr auto J_PORT_REQTYPE = "req_type";
constexpr auto J_PORT_REPTYPE = "rep_type";

constexpr auto J_PORT_QRYTYPE = "qry_type";
constexpr auto J_PORT_ANSTYPE = "ans_type";

constexpr auto J_PORT_TIMED   = "timed";
constexpr auto J_PORT_PERIOD  = "period";

constexpr auto J_TYPE         = "type";

constexpr auto J_GROUPS          = "groups";
constexpr auto J_GROUP_LEADER    = "leader";
constexpr auto J_GROUP_CONSENSUS = "consensus";
constexpr auto J_GROUP_MESSAGES  = "groupMessages";

#endif //RIAPS_CORE_R_JSONMODEL_H_H

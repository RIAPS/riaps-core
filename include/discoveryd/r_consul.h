#ifndef R_CONSUL
#define R_CONSUL

#define CONSUL_PORT 8500

#include <string>
#include "r_http_api.h"
#include "r_riaps_actor.h"
#include "json.h"
#include "utils/r_message.h"

void joinToCluster(std::string destination_ip);
void registerService(service_details& params);
void deregisterService(std::string service_name);
bool disc_getservices(std::vector<std::string>& service_list);
bool disc_getservicedetails(std::string, std::vector<service_details>&);
bool disc_deregisterservice(std::string);


bool disc_containskey(std::string keyname);
bool disc_registerkey(std::string key, std::string value);
bool disc_deregisterkey(std::string key, bool recurse);

bool disc_registernode(std::string nodename);
bool disc_deregisternode(std::string nodename);

bool disc_registeractor(std::string nodename, std::string actorname);
bool disc_deregisteractor(std::string nodename, std::string actorname);

std::string kv_hostnodekeypath();
std::string kv_nodekeypath(std::string nodename);

#endif
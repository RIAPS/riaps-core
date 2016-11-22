
#include "discoveryd/r_consul.h"

void consul_joinToCluster(std::string destination_address){
	std::string consul_api_host = "localhost";
	std::string consul_api_getparam = "/v1/agent/join/" + destination_address;

	std::string response;
	int result = do_get(consul_api_host, CONSUL_PORT, consul_api_getparam, response);
}

void registerService(service_details& params) {
	std::string consul_api_host = "localhost";
	std::string consul_api_getparam = "/v1/agent/service/register";

    // Create the JSON file
    nlohmann::json put_body;

    put_body["ID"]                = params.service_id;
    put_body["Name"]              = params.service_name;
    put_body["Tags"]              = params.tags;//std::vector<std::string>(service.GetServiceTags());
    put_body["Port"]              = std::stoi(params.port);
    put_body["Address"]           = params.ip_address;

    auto str_param = put_body.dump(1);

	int result = do_put(consul_api_host, CONSUL_PORT, consul_api_getparam, str_param);

	std::cout << "DO PUT RESULT: " << result << std::endl;
}

void deregisterService(std::string service_name){
    std::string consul_api_host = "localhost";
    std::string consul_api_getparam = "/v1/agent/service/deregister/" + service_name;

    std::string response;
    int result = do_get(consul_api_host, CONSUL_PORT, consul_api_getparam, response);
}

bool disc_getservices(std::vector<std::string>& service_list) {
	std::string consul_api_host = "localhost";
	std::string consul_api_getparam = "/v1/catalog/services";

	std::string response;
	int result = do_get(consul_api_host, CONSUL_PORT, consul_api_getparam, response);

	nlohmann::json json_response = nlohmann::json::parse(response.c_str());

	for (auto it = json_response.begin(); it != json_response.end(); ++it) {
		service_list.push_back(it.key());
    }

}

bool disc_getservicedetails(std::string servicename, std::vector<service_details>& service_list) {
    std::string consul_api_host = "localhost";
    std::string consul_api_getparam = "/v1/catalog/service/" + servicename;

    std::string response;
    int result = do_get(consul_api_host, CONSUL_PORT, consul_api_getparam, response);

    nlohmann::json json_response = nlohmann::json::parse(response.c_str());

    // Extract the details form each service instance
    for (auto it = json_response.begin(); it != json_response.end(); ++it) {
        //std::cout << (*it)["ServiceID"];
        service_details current_service;
        nlohmann::json current_item = (*it);
        current_service.ip_address   = current_item["ServiceAddress"];
        current_service.port         = std::to_string((uint)current_item["ServicePort"]);
        current_service.service_name = current_item["ServiceName"];
        current_service.service_id   = current_item["ServiceID"];

        for (auto tagit = current_item["Tags"].begin(); tagit!= current_item["Tags"].end(); tagit++){
            current_service.tags.push_back(*tagit);
        }

        service_list.push_back(current_service);
    }

    return true;
}

bool disc_deregisterservice(std::string servicename) {
    std::string consul_api_host = "localhost";
    std::string consul_api_getparam = "/v1/agent/service/deregister/" + servicename;

    std::string response;
    int result = do_get(consul_api_host, CONSUL_PORT, consul_api_getparam, response);

    return true;
}

bool disc_registerkey(std::string key, std::string value){
    std::string consul_api_host = "localhost";

    if (key.empty()) return false;

    if (key[0] == '/'){
        key = key.erase(0, 1);
    }

    std::string consul_api_getparam = "/v1/kv/" + key;

    int result = do_put(consul_api_host, CONSUL_PORT, consul_api_getparam, value);

    std::cout << "DO PUT RESULT: " << result << std::endl;
}

bool disc_registernode(std::string nodename){

    // Remove all leading and traling '/' or '\'
    auto pos = nodename.find('/');
    while(pos!=std::string::npos){
        nodename = nodename.erase(pos, 1);
        pos = nodename.find('/');
    }

    pos = nodename.find('\\');
    while(pos!=std::string::npos){
        nodename = nodename.erase(pos, 1);
        pos = nodename.find('\\');
    }



    return disc_registerkey(kv_nodekeypath(nodename), "");
}

bool disc_deregisternode(std::string nodename){
    // Remove all leading and traling '/' or '\'
    auto pos = nodename.find('/');
    while(pos!=std::string::npos){
        nodename = nodename.erase(pos, 1);
        pos = nodename.find('/');
    }

    pos = nodename.find('\\');
    while(pos!=std::string::npos){
        nodename = nodename.erase(pos, 1);
        pos = nodename.find('\\');
    }
    return disc_deregisterkey(kv_nodekeypath(nodename), true);
}

bool disc_deregisterkey(std::string key, bool recurse=true){
    std::string consul_api_host = "localhost";
    std::string consul_api_deleteparam = "/v1/kv/" + key;

    if (recurse){
        consul_api_deleteparam += "?recurse";
    }

    std::string response;
    int result = do_delete(consul_api_host, CONSUL_PORT, consul_api_deleteparam, response);

    std::cout << key << std::endl;
    std::cout << response << "," << result << std::endl;

    return true;
}

std::string disc_getvalue_by_key(std::string key) {
    std::string consul_api_host = "localhost";
    std::string consul_api_getparam = "/v1/kv/" + key;

    std::string value;
    std::string response;

    int result = do_get(consul_api_host, CONSUL_PORT, consul_api_getparam, response);

    nlohmann::json json_response = nlohmann::json::parse(response.c_str());

    if(!json_response.empty()){
        value = json_response[0]["Value"];
    }

    return value;
}


std::string kv_hostnodekeypath(){
    char* hostname = zsys_hostname();
    std::string hostname_str = std::string(hostname);
    free(hostname);

    return kv_nodekeypath(hostname);
}

std::string kv_nodekeypath(std::string nodename) {

    // Remove all leading and traling '/' or '\'
    auto pos = nodename.find('/');
    while(pos!=std::string::npos){
        nodename = nodename.erase(pos, 1);
        pos = nodename.find('/');
    }

    pos = nodename.find('\\');
    while(pos!=std::string::npos){
        nodename = nodename.erase(pos, 1);
        pos = nodename.find('\\');
    }

    std::string result = "nodes/" + std::string(nodename);
    return result;
}

std::string kv_actorkeypath(std::string nodename, std::string actorname){
    return kv_nodekeypath(nodename) + "/actors/" + actorname;
}

bool disc_registeractor(std::string nodename, std::string actorname){
    disc_registerkey(kv_actorkeypath(nodename, actorname), "");
}

bool disc_deregisteractor(std::string nodename, std::string actorname){
    disc_deregisterkey(kv_actorkeypath(nodename, actorname), true);
}
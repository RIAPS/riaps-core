//
// Created by parallels on 9/14/16.
//

#include <discoveryd/r_riaps_cmd_handler.h>
#include "componentmodel/r_network_interfaces.h"

void init_command_mappings() {
    handler_mapping[CMD_DISC_REGISTER_SERVICE]         = &handle_register_service;
    handler_mapping[CMD_DISC_DEREGISTER_SERVICE]       = &handle_deregister_service;
    //handler_mapping[CMD_DISC_GET_SERVICES]             = &handle_getservices;
    handler_mapping[CMD_DISC_GETSERVICE_BY_NAME]       = &handle_getservicebyname;
    handler_mapping[CMD_DISC_GETSERVICE_BY_NAME_ASYNC] = &handle_getservicebyname_async;
    handler_mapping[CMD_DISC_REGISTER_NODE]            = &handle_registernode;
    //handler_mapping[CMD_DISC_REGISTER_ACTOR]           = &handle_registeractor;
}


// TODO: return value
bool handle_command(std::string command, zmsg_t* msg, zsock_t* replysocket, zactor_t* asyncactor) {

    if (handler_mapping.find(command) == handler_mapping.end()){
        return false;
    }

    handler_mapping[command](msg, replysocket, asyncactor);
    return true;
}

void handle_register_service(zmsg_t* msg, zsock_t* replysocket, zactor_t* asyncactor) {
    std::vector<std::string> message_parameters;

    service_details service;
    extract_params(msg, message_parameters);
    params_to_service_details(message_parameters, service);

    registerService(service);
    zstr_send(replysocket, "REGISTERED");
}

void handle_deregister_service(zmsg_t* msg, zsock_t* replysocket, zactor_t* asyncactor) {
    char* service_name = zmsg_popstr(msg);
    if (service_name){
        deregisterService(std::string(service_name));
        zstr_send(replysocket, "DEREGISTERED");
        free(service_name);
    }
    else {
        std::cout << "no service name";
    }
}

/*
void handle_getservices(zmsg_t* msg, zsock_t* replysocket, zactor_t* asyncactor){
    std::vector<std::string> service_list;

    // Collect all the registered services
    disc_getservices(service_list);

    // Prepare the answer (in ZMQ message)
    zmsg_t* resultmsg = zmsg_new();
    zmsg_addstr(resultmsg, MSG_SERVICE_LIST_HEADER);

    // Add each servicename as an individual frame
    for(auto it = service_list.begin(); it!=service_list.end(); it++) {
        zmsg_addstr(resultmsg, it->c_str());
    }

    // Send it!
    zmsg_send(&resultmsg, replysocket);
}
 */

void handle_getservicebyname(zmsg_t* msg, zsock_t* replysocket, zactor_t* asyncactor){
    std::vector<service_details> service_list;
    char* servicename_param = zmsg_popstr(msg);

    if (servicename_param) {
        std::string servicename(servicename_param);
        disc_getservicedetails(servicename, service_list);
        free(servicename_param);

        // Prepare the answer
        // Create a message frame for each service

        // TODO: check is there a response in the list to send...
        zmsg_t *response_msg = zmsg_new();

        for (auto it = service_list.begin(); it != service_list.end(); it++) {
            zmsg_t *submessage = zmsg_new();
            service_details_to_zmsg(*it, submessage);
            zmsg_addmsg(response_msg, &submessage);
        }

        zmsg_send(&response_msg, replysocket);
    }
    else{
        // TODO send back empty message
    }
}

void handle_getservicebyname_async(zmsg_t* msg, zsock_t* replysocket, zactor_t* asyncactor){
    char* servicename_param = zmsg_popstr(msg);
    if (servicename_param) {
        char* returnaddress_param = zmsg_popstr(msg);
        if (returnaddress_param){

            // Pass the message to the ASYNC handler
            zsock_send(asyncactor, "ss", servicename_param, returnaddress_param);

            free(returnaddress_param);
        }
        free(servicename_param);

    }
    else{
        // TODO send back empty message
    }
    zstr_send(replysocket, "OK");
}

void handle_registernode(zmsg_t* msg, zsock_t* replysocket, zactor_t* asyncactor){
    // Register a node in the KV storage
    char* nodename = zmsg_popstr(msg);
    if (nodename) {
        std::string nodekey = "/nodes/" + std::string(nodename);

        //TODO: put ip address as value
        disc_registerkey(nodekey, "" );

        free(nodename);
        zstr_send(replysocket, "OK");
    }
}

// TODO: pass hostname somehow
void handle_registeractor(zmsg_t* msg, zsock_t* replysocket, zactor_t* asyncactor) {

}

std::pair<std::string, std::string>
buildInsertKeyValuePair(std::string appName ,
                        std::string msgType ,
                        Kind        kind    ,
                        Scope       scope   ,
                        std::string host    ,
                        uint16_t    port    ) {
    std::string key;
    key =   "/" + appName
          + "/" + msgType
          + "/" + kindMap[kind];

    if (scope == Scope::LOCAL){
        // hostid
        //auto hostid = gethostid();

        std::string mac_address = GetMacAddressStripped(RIAPS_DEFAULT_IFACE);

        key += mac_address;
    }

    std::string value = host + ":" + std::to_string(port);

    return std::pair<std::string, std::string>(key, value);
}

std::pair<std::string, std::string>
buildLookupKey(std::string appName,
               std::string msgType,
               Kind kind,
               Scope scope,
               std::string clientActorHost,
               std::string clientActorName,
               std::string clientInstanceName,
               std::string clientPortName ) {


    std::string key;

    std::map<Kind, std::string> kindPairs = {
                          {Kind::SUB, kindMap[Kind::PUB]},
                          {Kind::CLT, kindMap[Kind::SRV]},
                          {Kind::REQ, kindMap[Kind::REP]},
                          {Kind::REP, kindMap[Kind::REQ]}};

    key =     "/" + appName
            + "/" + msgType
            + "/" + kindPairs[kind];

    //auto hostid = gethostid();

    std::string hostid = GetMacAddressStripped(RIAPS_DEFAULT_IFACE);

    if (scope == Scope::LOCAL){
        key += hostid;
    }

    std::string client =   '/' + appName
                         + '/' + clientActorName
                         + '/' + clientActorHost
                         + '/' + clientInstanceName
                         + '/' + clientPortName;

    if (scope == Scope::LOCAL) {
        client = client + ":" + hostid;
    }

    return {key, client};
}
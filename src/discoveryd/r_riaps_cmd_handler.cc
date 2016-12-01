//
// Created by parallels on 9/14/16.
//

#include <discoveryd/r_riaps_cmd_handler.h>
#include "componentmodel/r_network_interfaces.h"

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

        std::string mac_address = GetMacAddressStripped();

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

    std::string hostid = GetMacAddressStripped();

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
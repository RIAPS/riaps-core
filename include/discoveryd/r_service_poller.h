//
// Created by parallels on 9/20/16.
//

#ifndef RIAPS_FW_R_SERVICE_POLLER_H
#define RIAPS_FW_R_SERVICE_POLLER_H


#include <czmq.h>
#include <string>
#include <iostream>
#include <vector>

#include "r_consul.h"

struct _service_query_params {
    std::string servicename;
    std::string replyaddress;
    int64_t     timestamp;
};

typedef struct _service_query_params service_query_params;

void service_poller_actor (zsock_t *pipe, void *args);

void execute_query(std::vector<service_query_params>& params);

#endif //RIAPS_FW_R_SERVICE_POLLER_H

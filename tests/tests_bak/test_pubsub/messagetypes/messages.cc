//
// Created by parallels on 9/15/16.
//

#include <czmq.h>
#include <string>
#include <vector>
#include "messages.h"





zmsg_t* create_message(std::string messagetype, std::vector<std::string>& params) {
    zmsg_t* result = zmsg_new();

    zmsg_addstr(result, messagetype.c_str());

    for (auto param : params) {
        zmsg_addstr(result, param.c_str());
    }


    return result;
}

void unpack_message(zmsg_t* msg, std::string& messagetype, std::vector<std::string>& params){
    bool has_more_msg = true;

    char* msgtype = zmsg_popstr(msg);

    if (msgtype) {
        messagetype.assign(msgtype);
        while (has_more_msg) {
            char *param = zmsg_popstr(msg);
            if (param) {
                params.push_back(param);
                free(param);
            } else {
                has_more_msg = false;
            }
        }

        free(msgtype);
    }
}
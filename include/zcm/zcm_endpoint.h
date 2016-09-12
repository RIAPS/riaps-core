//
// Created by parallels on 9/1/16.
//

#ifndef ZCM_ENDPOINT_H
#define ZCM_ENDPOINT_H

struct _zcm_endpoint{
    std::string address;
    std::string port;
};

typedef struct _zcm_endpoint zcm_endpoint;

#endif //ZCM_ENDPOINT_H

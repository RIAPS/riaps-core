//
// Created by istvan on 11/13/16.
//

#ifndef RIAPS_FW_R_CAPNP_HELPER_H
#define RIAPS_FW_R_CAPNP_HELPER_H

#include <capnp/serialize.h>
#include <capnp/common.h>
#include <czmq.h>
#include <string>

kj::ArrayPtr<const capnp::word> zmsg_to_arrayPtr(zmsg_t* msg);





#endif //RIAPS_FW_R_CAPNP_HELPER_H

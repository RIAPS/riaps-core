//
// Created by istvan on 11/13/16.
//

#include "utils/r_capnp_helper.h"

// Note
//  - delete .data() from caller
//  - delete msg from caller
kj::ArrayPtr<const capnp::word> zmsg_to_arrayPtr(zmsg_t* msg){
    zframe_t* capnp_msgbody = zmsg_pop(msg);
    size_t    size = zframe_size(capnp_msgbody);
    byte*     data = zframe_data(capnp_msgbody);

    auto capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));

    return capnp_data;
}

void sendLogRequest(std::string publisher_endpoint){

}

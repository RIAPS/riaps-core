#include <componentmodel/r_asyncinfo.h>
#include <czmq.h>
#include <zmsg.h>
#include <zmq.h>

riaps::AsyncInfo::AsyncInfo(const std::string& originId,
                            const std::string& messageId,
                            zmsg_t** created,
                            zmsg_t **expiration) : OriginId(originId), MessageId(messageId){
    //_deleter       = [](zmsg_t* z){zmsg_destroy(&z);};
    Created = *created;
    Expiration = *expiration;
}

riaps::AsyncInfo::~AsyncInfo() {
    zmsg_destroy(&Created);
    zmsg_destroy(&Expiration);

}
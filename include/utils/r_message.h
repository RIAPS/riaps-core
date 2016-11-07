
#ifndef R_MESSAGE
#define R_MESSAGE

#include <czmq.h>
#include <string>
#include <vector>

struct _service_details {
    std::string              service_id   ;
    std::string              service_name ;
    std::string              ip_address   ;
    std::string              port         ;
    std::vector<std::string> tags         ;

    bool operator==( _service_details const& rhs ){
        if (service_name == rhs.service_name &&
            ip_address   == rhs.ip_address   &&
            port         == rhs.port         &&
            service_id   == rhs.service_id){
            return true;
        }
        return false;
    }
};

typedef struct _service_details service_details;



void
extract_zmsg(zmsg_t* msg, std::vector<zmsg_t*>& msg_frames);

void
extract_params(zmsg_t* msg, std::vector<std::string>& params);

bool
params_to_service_details(std::vector<std::string>& params,
                          service_details&          service_structure);

void
service_details_to_zmsg(service_details& current_service, zmsg_t* msg);


#endif //_R_MESSAGE_H

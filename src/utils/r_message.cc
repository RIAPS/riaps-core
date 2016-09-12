#include "utils/r_message.h"

void
extract_zmsg(zmsg_t* msg, std::vector<zmsg_t*>& msg_frames) {
    bool has_more_frame = true;

    while (has_more_frame){
        zmsg_t* submessage = zmsg_popmsg(msg);
        if (submessage){
            msg_frames.push_back(submessage);
        } else{
            has_more_frame = false;
        }
    }
}

void
extract_params(zmsg_t* msg, std::vector<std::string>& params) {
    bool has_more_msg = true;

    while (has_more_msg){
        char* param = zmsg_popstr(msg);
        if (param){
            std::string param_str;
            param_str.assign(param);
            params.push_back(param_str);
            free(param);
        } else{
            has_more_msg = false;
        }
    }
}

bool
params_to_service_details(std::vector<std::string>& params,
                          service_details&          service_structure) {

    // 0 -> service_id
    // 1 -> service_name
    // 2 -> ip_address
    // 3 -> port
    // 4 -> tags (starts with $TAGS$ message)

    int current_index = 0;
    bool has_tags = false;
    for (auto it = params.begin(); it!=params.end(); it++){
        switch(current_index){
            // service_id
            case 0:
                service_structure.service_id.assign(*it);
                break;

                // service_name
            case 1:
                service_structure.service_name.assign(*it);
                break;

                // ip address
            case 2:
                service_structure.ip_address.assign(*it);
                break;

                // port
            case 3:
                service_structure.port.assign(*it);
                break;

                // $TAGS$
            case 4:
                if (*it!="$TAGS$")
                    return false;
                has_tags=true;
                break;

            default:
                if (has_tags) {
                    service_structure.tags.push_back(*it);
                }
        }

        current_index++;
    }

    if (current_index<4)
        return false;

    return true;
};

void
service_details_to_zmsg(service_details& current_service, zmsg_t* msg){
    // 0 -> service_id
    // 1 -> service_name
    // 2 -> ip_address
    // 3 -> port
    // 4 -> tags (starts with $TAGS$ message)

    zmsg_addstr(msg, current_service.service_id.c_str());
    zmsg_addstr(msg, current_service.service_name.c_str());
    zmsg_addstr(msg, current_service.ip_address.c_str());
    zmsg_addstr(msg, current_service.port.c_str());

    zmsg_addstr(msg, "$TAGS$");
    for (auto it = current_service.tags.begin(); it!=current_service.tags.end(); it++){
        zmsg_addstr(msg, it->c_str());
    }
}
#include <capnp/serialize.h>
#include <kj/common.h>
#include "utils/r_utils.h"

void operator<<(zmsg_t*& zmsg, capnp::MallocMessageBuilder& message){
    auto serializedMessage = capnp::messageToFlatArray(message);
    zmsg = zmsg_new();
    auto bytes = serializedMessage.asBytes();
    zmsg_pushmem(zmsg, bytes.begin(), bytes.size());
}

void operator>>(zframe_t& frame, capnp::FlatArrayMessageReader*& message){
    size_t size      = zframe_size(&frame);
    byte* data = zframe_data(&frame);
    kj::ArrayPtr<const capnp::word> capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));
    message = new capnp::FlatArrayMessageReader(capnp_data);
}


void print_cacheips(std::map<std::string, int64_t>& ipcache) {
    std::cout << "Stored ips: ";
    for (auto it=ipcache.begin(); it!=ipcache.end(); it++){
        std::cout << it->first << "; ";
    }
    std::cout << std::endl;
}

bool maintain_cache(std::map<std::string, int64_t>& ipcache){
    bool is_maintained = false;
    int64_t time = zclock_mono();

    // Maintain the list, if somebody didn't respond in the past IPCACHE_TIMEOUT seconds => remove from the list
    std::vector<std::string> marked_for_delete;
    for (auto it=ipcache.begin(); it!=ipcache.end(); it++){
        if ((time-it->second)>IPCACHE_TIMEOUT)
            marked_for_delete.push_back(it->first);
    }

    // Remove marked elements
    for (auto it=marked_for_delete.begin(); it!=marked_for_delete.end(); it++){
        ipcache.erase(*it);
        is_maintained = true;
    }

    return is_maintained;
}

std::vector<std::string> maintain_servicecache(std::map<std::string, int64_t >& servicecache){
    int64_t time = zclock_mono();

    // Maintain the list, if somebody didn't respond in the past 10 seconds => remove from the list
    std::vector<std::string> outdated_services;
    for (auto it=servicecache.begin(); it!=servicecache.end(); it++){
        if ((time-it->second)>10000)
            outdated_services.push_back(it->first);
    }

    // Remove marked elements
    for (auto it=outdated_services.begin(); it!=outdated_services.end(); it++){
        servicecache.erase(*it);
    }

    // deregister services from discovery service
    if (outdated_services.size()>0) {
        for (auto outdated_service : outdated_services) {

            //TODO: DEREGISTER SOMEWHERE ELSE!
           // disc_deregisterservice(outdated_service);
        }
    }

    return outdated_services;
}

const std::string GetAppPath(const std::string& appName){
    char* riapsAppsPath = std::getenv(ENV_RIAPSAPPS);
    
    if (riapsAppsPath == nullptr) return "";
    
    std::string p = riapsAppsPath;
    if (p.back() == '/')
        p.pop_back();
    p+= "/" + appName;
    
    return p;
}


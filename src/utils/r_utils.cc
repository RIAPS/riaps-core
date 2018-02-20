#include <capnp/serialize.h>
#include <kj/common.h>
#include "utils/r_utils.h"

void operator<<(zmsg_t*& zmsg, capnp::MallocMessageBuilder& message){
    auto serializedMessage = capnp::messageToFlatArray(message);
    zmsg = zmsg_new();
    auto bytes = serializedMessage.asBytes();
    zmsg_pushmem(zmsg, bytes.begin(), bytes.size());
}

void operator<<(zframe_t*& zframe, capnp::MallocMessageBuilder& message){
    auto serializedMessage = capnp::messageToFlatArray(message);
    auto bytes = serializedMessage.asBytes();
    zframe = zframe_new(bytes.begin(), bytes.size());
}


void operator>>(zframe_t& frame, capnp::FlatArrayMessageReader*& message){
    size_t size      = zframe_size(&frame);
    byte* data = zframe_data(&frame);
    kj::ArrayPtr<const capnp::word> capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));
    message = new capnp::FlatArrayMessageReader(capnp_data);
}

void operator>>(zframe_t& frame, std::unique_ptr<capnp::FlatArrayMessageReader>& message){
    size_t size      = zframe_size(&frame);
    byte* data = zframe_data(&frame);
    kj::ArrayPtr<const capnp::word> capnp_data = kj::arrayPtr(reinterpret_cast<const capnp::word*>(data), size / sizeof(capnp::word));
    message.reset(new capnp::FlatArrayMessageReader(capnp_data));
}

namespace spd=spdlog;

void print_cacheips(std::map<std::string, riaps::utils::Timeout>& ipcache) {
    auto logger = spd::get("rdiscovery");
    if (logger == nullptr)
        return;

    fmt::MemoryWriter w;

    w.write("Known nodes: ");
    for (auto &it : ipcache) {
        w << it.first << "; ";
    }
    logger->info(w.c_str());
}

bool maintain_cache(std::map<std::string, riaps::utils::Timeout>& ipcache){
    bool is_maintained = false;

    // Maintain the list, if somebody didn't respond in the past IPCACHE_TIMEOUT seconds => remove from the list
    std::vector<std::string> marked_for_delete;
    for (auto &it : ipcache) {
        if (it.second.IsTimeout())
            marked_for_delete.push_back(it.first);
    }

    // Remove marked elements
    for (auto it=marked_for_delete.begin(); it!=marked_for_delete.end(); it++){
        ipcache.erase(*it);
        is_maintained = true;
    }

//    int64_t time = zclock_mono();
//
//    // Maintain the list, if somebody didn't respond in the past IPCACHE_TIMEOUT seconds => remove from the list
//    std::vector<std::string> marked_for_delete;
//    for (auto &it : ipcache) {
//        if ((time- it.second)>IPCACHE_TIMEOUT)
//            marked_for_delete.push_back(it.first);
//    }
//
//    // Remove marked elements
//    for (auto it=marked_for_delete.begin(); it!=marked_for_delete.end(); it++){
//        ipcache.erase(*it);
//        is_maintained = true;
//    }

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

timespec operator-(const timespec& ts1, const timespec& ts2){
    auto diffSec = ts1.tv_sec   - ts2.tv_sec;
    auto diffNSec = ts1.tv_nsec - ts2.tv_nsec;

    if (diffNSec<0) {
        diffSec--;
        diffNSec+=1000000000l; // BILLION
    }

    if (diffSec<0) {
        diffSec++;
        diffNSec-=1000000000l;
    }

    timespec result;
    result.tv_sec  = diffSec;
    result.tv_nsec = diffNSec;

    return result;
}

bool operator>(const timespec& ts1, const timespec& ts2) {
    if (ts1.tv_sec>ts2.tv_sec) return true;
    if (ts1.tv_sec == ts2.tv_sec && ts1.tv_nsec>ts2.tv_nsec) return true;
    return false;
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


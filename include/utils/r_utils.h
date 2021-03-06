/*!
 * Helper functions to maintain and debug the stored IP addresses from UDP beacons.
 *
 * \author Istvan Madari
 */


#ifndef R_UTILS
#define R_UTILS 


#include <const/r_const.h>
#include <utils/r_timeout.h>

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <czmq.h>
#include <spdlog_setup/conf.h>
#include <opendht.h>

#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <vector>

// For how long the ip addresses are stored in a local cache.
// The ip addresses are obtained from the UDP beacons
#define IPCACHE_TIMEOUT 30000 // millisec

/**
 * Prints the cached IP addresses to the standard output
 * 
 * @param ipcache <IP address, timestamp> pairs
 */
void print_cacheips(std::map<std::string, riaps::utils::Timeout<std::chrono::milliseconds>>& ipcache, const std::string& selfAddress);


/**
 * Removes entries from the IP CACHE if no package from the node for IPCACHE_TIMEOUT.
 * 
 * @param ipcache <IP address, timestamp> pairs
 * @return true if entries are removed from the cache
 */

bool maintain_cache(std::map<std::string, riaps::utils::Timeout<std::chrono::milliseconds>>& ipcache);

/**
 * @deprecated
 */
std::vector<std::string> maintain_servicecache(std::map<std::string, int64_t >& servicecache);

/**
 * Returns the full path of the given RIAPS application. The full path is the concatenated string of RIAPSHOME env.
 * variable and appName, without '/' at the end. The function does NOT check whether the path is valid.
 *
 * @param appName Name of the RIAPS application.
 * @return The full path or empty string if the RIAPSHOME is not set.
 */
const std::string GetAppPath(const std::string& appName);

void operator<<(zmsg_t*&   zmsg, capnp::MallocMessageBuilder& message);
void operator<<(zframe_t*& zframe, capnp::MallocMessageBuilder& message);



void operator>>(zframe_t& frame, capnp::FlatArrayMessageReader*& message);
void operator>>(zframe_t& frame, std::unique_ptr<capnp::FlatArrayMessageReader>& message);
void operator>>(zframe_t& frame, std::shared_ptr<capnp::FlatArrayMessageReader>& message);
void operator>>(zframe_t& frame, capnp::FlatArrayMessageReader& message);

timespec operator-(const timespec& ts1, const timespec& ts2);
bool operator>(const timespec& ts1, const timespec& ts2);

template<class T>
dht::Blob ConvertToBlob(T* data, size_t size) {
    dht::Blob result;
    transform(data, data+size, back_inserter(result),
              [](T c) -> uint8_t {
                  return (uint8_t)c;
              });
    return result;
}


dht::Blob ConvertToBlob(const std::string& data);

#endif
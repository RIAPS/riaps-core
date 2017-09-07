#ifndef R_UTILS
#define R_UTILS 

#include <iostream>
#include <string>
#include <map>
#include <czmq.h>
#include <vector>

// For how long the ip addresses are stored in a local cache.
// The ip addresses are obtained from the UDP beacons
#define IPCACHE_TIMEOUT 30000 // msec

/// \brief Prints the stored IP addresses to the standard output
/// \param ipcache
void print_cacheips(std::map<std::string, int64_t>& ipcache);


/// \brief Removes entries if no package from the node for IPCACHE_TIMEOUT
/// \param ipcache IP address - timestamp pairs
/// \return true if entries are removed from the cache
bool maintain_cache(std::map<std::string, int64_t>& ipcache);

/// \deprecated
std::vector<std::string> maintain_servicecache(std::map<std::string, int64_t >& servicecache);

#endif
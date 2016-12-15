#ifndef R_UTILS
#define R_UTILS 

#include <iostream>
#include <string>
#include <map>
#include <czmq.h>
#include <vector>


void print_cacheips(std::map<std::string, int64_t>& ipcache);

bool maintain_cache(std::map<std::string, int64_t>& ipcache);

std::vector<std::string> maintain_servicecache(std::map<std::string, int64_t >& servicecache);

/*
std::string trimend(std::string string, char character);
std::string trimbegin(std::string string, char character);
std::string trim(std::string, char character);
*/

#endif
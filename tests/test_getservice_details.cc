#include <czmq.h>
#include <iostream>
#include "../include/r_framework.h"

int main() {
    std::vector<service_details> results;

    bool error = get_servicebyname("TestService3", results);

    for (auto it=results.begin(); it!=results.end(); it++){
        std::cout << it->service_name << "; " << it->ip_address << std::endl;

        std::cout << "Tags:" << std::endl;

        for (auto tagit = it->tags.begin(); tagit!=it->tags.end(); tagit++){
            std::cout << "    - " << *tagit << std::endl;
        }
    }
}

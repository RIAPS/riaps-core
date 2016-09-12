#include <czmq.h>
#include <iostream>
#include "../include/r_framework.h"

int main() {
    std::vector<std::string> results;

    bool error = get_servicenames(results);

    for (auto it=results.begin(); it!=results.end(); it++){
        std::cout << *it << std::endl;
    }
}

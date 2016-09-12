#include <czmq.h>
#include <iostream>
#include "../include/r_framework.h"

int main() {
    std::vector<std::string> tags = {"alma", "korte"};
    std::cout << "Returned: " << register_service("serviceId3",
                                                  "TestService3",
                                                  "192.168.1.102",
                                                  "7861",
                                                  tags);
}
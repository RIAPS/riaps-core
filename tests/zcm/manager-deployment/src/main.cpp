#include "manager.hpp"

int main() {

  zcm::Manager manager("manager_config.json"); 
  manager.deploy();
  
  return 0;
}


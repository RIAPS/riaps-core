#include "../../../../../include/zcm/actor.hpp"

int main() {

  zcm::Actor subscribe_actor;
  subscribe_actor.configure("sub_conf.json");
  subscribe_actor.run();
  
  return 0;
}
